// ============================================================
//  scan_matching.hpp - 2D scan matching for the pose graph front end (no ROS)
//
//  pl_icp       point-to-line ICP (Censi, "An ICP variant using a point-to-line
//               metric", ICRA 2008): residual = distance from a moved source point
//               to the TANGENT LINE at its nearest reference point. Point-to-point
//               ICP stops at the beam spacing (~1-4 cm here); point-to-line slides
//               along walls and converges to mm on structured scans.
//  correlative  exhaustive search of a pose window on a blurred likelihood grid
//               (Olson, "Real-time correlative scan matching", ICRA 2009) - a
//               global-in-the-window initial guess for loop closures, where the
//               drift can be far outside ICP's convergence basin.
//  Poses are (x, y, theta); a pose T maps source-frame points into the reference frame.
// ============================================================
#pragma once
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace sm {

using Vec2 = Eigen::Vector2d;

inline double wrap(double a) { return std::atan2(std::sin(a), std::cos(a)); }

struct Pose2 {
    double x = 0, y = 0, th = 0;
    Vec2 apply(const Vec2& p) const {
        const double c = std::cos(th), s = std::sin(th);
        return {x + c * p.x() - s * p.y(), y + s * p.x() + c * p.y()};
    }
    Pose2 operator*(const Pose2& b) const {           // compose: this then b
        const Vec2 t = apply({b.x, b.y});
        return {t.x(), t.y(), wrap(th + b.th)};
    }
    Pose2 inverse() const {
        const double c = std::cos(th), s = std::sin(th);
        return {-(c * x + s * y), -(-s * x + c * y), -th};
    }
};
inline Pose2 between(const Pose2& a, const Pose2& b) { return a.inverse() * b; }   // a^-1 b

// Points with unit normals (normal valid = false where the local surface is unclear)
struct Cloud {
    std::vector<Vec2> p, n;
    std::vector<char> has_n;
    size_t size() const { return p.size(); }
};

// Normals by PCA over neighbours in SCAN ORDER (+-k points, each within the allowed gap per step of the centre).
// The allowed gap grows with range (2.5 beam spacings, at least max_gap): far points of a sparse scan
// still get normals (a fixed 10 cm left points beyond ~8 m without normals, so they could never match).
inline void compute_normals(Cloud& c, int k = 2, double max_gap = 0.10, double angular_res = 0.0126)
{
    const int N = static_cast<int>(c.p.size());
    c.n.assign(N, Vec2(0, 0)); c.has_n.assign(N, 0);
    for (int i = 0; i < N; ++i) {
        const double gap = std::max(max_gap, 2.5 * c.p[i].norm() * angular_res);
        Vec2 m(0, 0); int cnt = 0;
        for (int j = std::max(0, i - k); j <= std::min(N - 1, i + k); ++j)
            if ((c.p[j] - c.p[i]).norm() < gap * std::max(1, std::abs(j - i))) { m += c.p[j]; ++cnt; }
        if (cnt < 3) continue;
        m /= cnt;
        Eigen::Matrix2d C = Eigen::Matrix2d::Zero();
        for (int j = std::max(0, i - k); j <= std::min(N - 1, i + k); ++j)
            if ((c.p[j] - c.p[i]).norm() < gap * std::max(1, std::abs(j - i))) C += (c.p[j] - m) * (c.p[j] - m).transpose();
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> es(C);
        if (es.eigenvalues()(1) < 1e-12 || es.eigenvalues()(0) > 0.1 * es.eigenvalues()(1)) continue;   // not line-like
        c.n[i] = es.eigenvectors().col(0); c.has_n[i] = 1;
    }
}

// Uniform-grid nearest neighbour (reference cloud is fixed during one match)
class Grid {
public:
    Grid(const Cloud& c, double cell) : c_(c), cell_(cell) {
        for (size_t i = 0; i < c.size(); ++i) if (c.has_n[i]) map_[key(cell_idx(c.p[i].x()), cell_idx(c.p[i].y()))].push_back(static_cast<int>(i));
    }
    // nearest reference point (with a normal) within r <= cell; -1 if none
    int nearest(const Vec2& q, double r, double& d2best) const {
        const long cx = cell_idx(q.x()), cy = cell_idx(q.y()); int best = -1; d2best = r * r;
        for (long dx = -1; dx <= 1; ++dx) for (long dy = -1; dy <= 1; ++dy) {
            auto it = map_.find(key(cx + dx, cy + dy)); if (it == map_.end()) continue;
            for (int i : it->second) { const double d2 = (c_.p[i] - q).squaredNorm(); if (d2 < d2best) { d2best = d2; best = i; } }
        }
        return best;
    }
private:
    long cell_idx(double v) const { return static_cast<long>(std::floor(v / cell_)); }
    static long long key(long a, long b) { return (static_cast<long long>(a) << 32) ^ (b & 0xffffffffLL); }
    const Cloud& c_; double cell_;
    std::unordered_map<long long, std::vector<int>> map_;
};

struct IcpResult {
    Pose2 T; double rms = 1e9, overlap = 0; int iterations = 0; bool ok = false;
    Eigen::Matrix3d H = Eigen::Matrix3d::Zero();     // sum J'J over inliers (unit weights)
};

// Point-to-line ICP. Correspondence gate shrinks 0.3 -> 0.15 -> 0.08 m (coarse to fine), but never below
// 1.5 beam spacings at the point's range: neighbouring points of a scan are range * angular_res apart
// (LD06 0.72 deg: 7.5 cm at 6 m, 10 cm at 8 m), so a fixed 8 cm gate dropped correct far matches and
// under-counted overlap in open areas (found 2026-10-18, big arena: 63 good matches rejected at one junction).
inline IcpResult pl_icp(const Cloud& ref, const Cloud& src, Pose2 T, int max_iters = 40, double overlap_dist = 0.08,
                        double angular_res = 0.0126)
{
    auto gate = [&](const Vec2& p, double base) { return std::min(0.3, std::max(base, 1.5 * p.norm() * angular_res)); };
    IcpResult res; res.T = T;
    if (ref.size() < 20 || src.size() < 20) return res;
    Grid g(ref, 0.3);
    for (int it = 0; it < max_iters; ++it) {
        const double dmax = it < 8 ? 0.3 : (it < 16 ? 0.15 : 0.08);
        Eigen::Matrix3d H = Eigen::Matrix3d::Zero(); Eigen::Vector3d b = Eigen::Vector3d::Zero();
        const double c = std::cos(T.th), s = std::sin(T.th); int n = 0;
        for (const auto& p : src.p) {
            const Vec2 q = T.apply(p); double d2;
            const int j = g.nearest(q, gate(p, dmax), d2); if (j < 0) continue;
            const Vec2& nr = ref.n[j];
            const double r = nr.dot(q - ref.p[j]);
            const Eigen::Vector3d J(nr.x(), nr.y(), nr.dot(Vec2(-s * p.x() - c * p.y(), c * p.x() - s * p.y())));
            const double w = std::abs(r) < 0.02 ? 1.0 : 0.02 / std::abs(r);     // Huber, k = 2 cm
            H += w * J * J.transpose(); b += w * J * r; ++n;
        }
        if (n < 20) return res;
        const Eigen::Vector3d dx = H.ldlt().solve(-b);
        T.x += dx(0); T.y += dx(1); T.th = wrap(T.th + dx(2)); res.iterations = it + 1;
        if (it >= 16 && dx.head<2>().norm() < 1e-5 && std::abs(dx(2)) < 1e-5) break;
    }
    // final statistics at the solution
    const double c = std::cos(T.th), s = std::sin(T.th); double ss = 0; int n = 0, nin = 0;
    for (const auto& p : src.p) {
        const Vec2 q = T.apply(p); double d2;
        const int j = g.nearest(q, gate(p, overlap_dist), d2); if (j < 0) continue;
        const Vec2& nr = ref.n[j]; const double r = nr.dot(q - ref.p[j]);
        const Eigen::Vector3d J(nr.x(), nr.y(), nr.dot(Vec2(-s * p.x() - c * p.y(), c * p.x() - s * p.y())));
        res.H += J * J.transpose(); ss += r * r; ++n; if (std::abs(r) < 0.03) ++nin;
    }
    res.T = T;
    if (n < 20) return res;
    res.rms = std::sqrt(ss / n);
    res.overlap = static_cast<double>(nin) / static_cast<double>(src.size());
    res.ok = true;
    return res;
}

// Correlative search: score(T) = mean over source points of a Gaussian-blurred reference
// occupancy (sigma), evaluated for every T in the window around `centre`. Returns best T and score in [0, 1].
inline Pose2 correlative(const Cloud& ref, const Cloud& src, const Pose2& centre, double win_xy, double win_th,
                         double& best_score, double res = 0.03, double step_th = 0.015, double sigma = 0.05)
{
    best_score = 0; Pose2 best = centre;
    if (ref.size() < 20 || src.size() < 20) return best;
    double x0 = 1e9, y0 = 1e9, x1 = -1e9, y1 = -1e9;
    for (const auto& p : ref.p) { x0 = std::min(x0, p.x()); y0 = std::min(y0, p.y()); x1 = std::max(x1, p.x()); y1 = std::max(y1, p.y()); }
    const double m = 3 * sigma; x0 -= m; y0 -= m; x1 += m; y1 += m;
    const int W = static_cast<int>((x1 - x0) / res) + 1, Hh = static_cast<int>((y1 - y0) / res) + 1;
    std::vector<float> L(static_cast<size_t>(W) * Hh, 0.f);
    const int kr = static_cast<int>(std::ceil(m / res));
    for (const auto& p : ref.p) {
        const int cx = static_cast<int>((p.x() - x0) / res), cy = static_cast<int>((p.y() - y0) / res);
        for (int dy = -kr; dy <= kr; ++dy) for (int dx = -kr; dx <= kr; ++dx) {
            const int X = cx + dx, Y = cy + dy; if (X < 0 || Y < 0 || X >= W || Y >= Hh) continue;
            const double ex = x0 + (X + 0.5) * res - p.x(), ey = y0 + (Y + 0.5) * res - p.y();
            float& v = L[static_cast<size_t>(Y) * W + X];
            v = std::max(v, static_cast<float>(std::exp(-(ex * ex + ey * ey) / (2 * sigma * sigma))));
        }
    }
    // decimate the source to <= 200 points for speed
    std::vector<Vec2> S; const size_t stride = std::max<size_t>(1, src.size() / 200);
    for (size_t i = 0; i < src.size(); i += stride) S.push_back(src.p[i]);
    const int nxy = static_cast<int>(std::round(win_xy / res)), nth = static_cast<int>(std::round(win_th / step_th));
    std::vector<Vec2> R(S.size());
    for (int k = -nth; k <= nth; ++k) {
        const double th = centre.th + k * step_th, c = std::cos(th), s = std::sin(th);
        for (size_t i = 0; i < S.size(); ++i) R[i] = Vec2(c * S[i].x() - s * S[i].y(), s * S[i].x() + c * S[i].y());
        for (int iy = -nxy; iy <= nxy; ++iy) for (int ix = -nxy; ix <= nxy; ++ix) {
            const double tx = centre.x + ix * res, ty = centre.y + iy * res; double sc = 0;
            for (const auto& q : R) {
                const int X = static_cast<int>((q.x() + tx - x0) / res), Y = static_cast<int>((q.y() + ty - y0) / res);
                if (X >= 0 && Y >= 0 && X < W && Y < Hh) sc += L[static_cast<size_t>(Y) * W + X];
            }
            sc /= static_cast<double>(S.size());
            if (sc > best_score) { best_score = sc; best = {tx, ty, wrap(th)}; }
        }
    }
    return best;
}

}  // namespace sm
