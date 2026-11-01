// ============================================================
//  GraphBuilderNode - pose graph FRONT END
//
//  Same structure as slam_toolbox / Karto and Cartographer (scan matching
//  refines odometry, loop closures are verified scan matches, the back end
//  only optimises):
//
//   scan + odom      a scan becomes a NODE when the robot moved >= min_node_dist
//                    or turned >= min_node_rot since the last node. Its odometry
//                    pose is interpolated AT THE SCAN TIME; the points are moved
//                    into the BASE frame (lidar mounting offset from TF) and
//                    deskewed when the scan carries time_increment.
//   moving objects   before matching, points sitting where an older node's scan looked PAST them (free space
//                    then, occupied now) are dropped: they moved in. In a long corridor the walls say nothing
//                    along it, so such points otherwise steer ICP. Dropped points go to /moving_points (map
//                    frame, one cloud per node). Compares along the chain of sequential steps, so the result
//                    does not depend on when a back end result arrives.
//   sequential edges odometry edge (motion-model covariance) AND a point-to-line
//                    ICP edge against a local map of the last nodes, started from
//                    odometry (Censi 2008). Both go to the back end.
//   loop closures    candidates near the current pose estimate -> correlative
//                    search over a window (Olson 2009) -> point-to-line ICP ->
//                    accepted on overlap + residual. Information = ICP Hessian
//                    / rms^2 (checked consistent on synthetic scans).
//   estimates        every node keeps a pose estimate: optimised poses from the
//                    back end, newer nodes chained on with their sequential edge.
//                    The back end starts from these (warm start).
//   map -> odom TF   from the newest node: est * odom^-1 (the unperturbed /odom pose).
//   /odom_path       the node odometry alone (drift) for comparison with /optimized_path.
//   node scans       published once per node on /node_scans for the map node.
//
//  odom_noise_enabled: perturbs the odometry INCREMENTS (so edges and initial
//  guesses both drift, as with real odometry) - a fair test of the front end.
// ============================================================
#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/path.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/utils.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/transform_broadcaster.h>
#include <slam_msgs/msg/pose_graph.hpp>
#include <slam_msgs/msg/node_scan.hpp>
#include <cstring>
#include <deque>
#include <mutex>
#include <random>
#include "scan_matching.hpp"

using sm::Pose2; using sm::Cloud; using sm::Vec2; using sm::between; using sm::wrap;

struct GraphNode {
    int id; double t;
    Pose2 odom;      // odometry pose at the scan time (noisy when odom_noise_enabled)
    Pose2 odom_msg;  // the /odom message pose at the scan time (for the map -> odom TF)
    Pose2 est;       // current estimate (map frame)
    Pose2 seq_z;     // sequential relative pose used to chain this node onto the previous one
    Pose2 chain;     // pose along the chain of seq_z: unlike est, never changes afterwards
    double travel = 0;  // path length driven up to this node (m)
    std::vector<float> polar;  // nearest return per beam in the lidar frame, full range, 0 = no return (free-space filter)
    Cloud scan;      // base frame, deskewed, with normals
};

class GraphBuilderNode : public rclcpp::Node {
public:
    GraphBuilderNode() : Node("graph_builder_node"), rng_(std::random_device{}())
    {
        odom_topic_  = declare_parameter("odom_topic", std::string("/odom"));
        scan_topic_  = declare_parameter("scan_topic", std::string("/scan"));
        map_frame_   = declare_parameter("map_frame", std::string("map"));
        base_frame_  = declare_parameter("base_frame", std::string(""));            // "" = odom child_frame_id
        publish_tf_  = declare_parameter("publish_map_odom_tf", true);
        stamp_end_   = declare_parameter("scan_stamp_at_end", false);  // LD06 driver stamps the END of the rotation; ROS convention = first beam
        deskew_      = declare_parameter("deskew", true);
        min_range_   = declare_parameter("min_range", 0.20);           // m - drops returns from the robot's own body
        max_range_   = declare_parameter("max_range", 8.0);
        min_dist_    = declare_parameter("min_node_dist", 0.2);
        min_rot_     = declare_parameter("min_node_rot", 0.3);         // rad
        // odometry edge covariance: sigma_t = sqrt((kt d)^2 + (ktr a)^2 + ft^2), sigma_r = sqrt((krt d)^2 + (kr a)^2 + fr^2)
        kt_  = declare_parameter("odom_sigma_trans_per_m", 0.05);
        ktr_ = declare_parameter("odom_sigma_trans_per_rad", 0.02);
        krt_ = declare_parameter("odom_sigma_rot_per_m", 0.05);
        kr_  = declare_parameter("odom_sigma_rot_per_rad", 0.05);
        ft_  = declare_parameter("odom_sigma_trans_floor", 0.005);
        fr_  = declare_parameter("odom_sigma_rot_floor", 0.005);
        // moving-object filter: nodes back to compare with, margin (m), +-beams
        dyn_back_   = declare_parameter<std::vector<int64_t>>("dynamic_filter_back", {4, 8, 12, 16});     // empty = off
        dyn_margin_ = declare_parameter("dynamic_filter_margin", 0.3);
        dyn_tol_    = declare_parameter("dynamic_filter_tol", 1);
        // sequential scan matching
        seq_on_       = declare_parameter("scan_match_sequential", true);
        seq_nodes_    = declare_parameter("seq_local_map_nodes", 3);
        seq_overlap_  = declare_parameter("seq_min_overlap", 0.7);
        seq_rms_      = declare_parameter("seq_max_rms", 0.03);
        seq_disagree_ = declare_parameter("seq_max_disagree", 0.15);   // m and rad vs odometry
        icp_floor_    = declare_parameter("icp_sigma_floor", 0.005);    // m - rms floor for the information
        icp_scale_    = declare_parameter("icp_info_scale", 0.5);       // 0.5 = covariance x2 (safety)
        // loop closures
        lc_min_gap_   = declare_parameter("lc_min_node_gap", 20);
        // AND >= this much driving since the candidate: a 360 deg spin in place makes ~21 nodes at one spot,
        // so a node-count gap alone let a spin 'close a loop' with itself (found 2026-10-18, big arena corners)
        lc_min_travel_ = declare_parameter("lc_min_travel", 5.0);
        lc_cooldown_  = declare_parameter("lc_cooldown", 5);
        lc_dist_      = declare_parameter("lc_search_dist", 1.0);
        lc_cands_     = declare_parameter("lc_max_candidates", 3);
        lc_submap_    = declare_parameter("lc_submap_half", 2);         // candidate +- n nodes form the reference
        lc_win_xy_    = declare_parameter("lc_window_xy", 0.6);
        lc_win_th_    = declare_parameter("lc_window_th", 0.4);
        lc_score_     = declare_parameter("lc_min_score", 0.6);
        lc_overlap_   = declare_parameter("lc_min_overlap", 0.7);
        lc_rms_       = declare_parameter("lc_max_rms", 0.03);
        // optional odometry noise (testing)
        noise_on_ = declare_parameter("odom_noise_enabled", false);
        a1_ = declare_parameter("noise_alpha1", 0.02); a2_ = declare_parameter("noise_alpha2", 0.005);
        a3_ = declare_parameter("noise_alpha3", 0.005); a4_ = declare_parameter("noise_alpha4", 0.005);

        tf_buffer_ = std::make_unique<tf2_ros::Buffer>(get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        tf_pub_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
        odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(odom_topic_, 50,
            std::bind(&GraphBuilderNode::odom_cb, this, std::placeholders::_1));
        scan_sub_ = create_subscription<sensor_msgs::msg::LaserScan>(scan_topic_, rclcpp::SensorDataQoS(),
            std::bind(&GraphBuilderNode::scan_cb, this, std::placeholders::_1));
        path_sub_ = create_subscription<nav_msgs::msg::Path>("/optimized_path", 10,
            std::bind(&GraphBuilderNode::path_cb, this, std::placeholders::_1));
        graph_pub_ = create_publisher<slam_msgs::msg::PoseGraph>("/pose_graph", 10);
        odom_path_pub_ = create_publisher<nav_msgs::msg::Path>("/odom_path", 10);
        moved_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/moving_points", 10);
        node_scan_pub_ = create_publisher<slam_msgs::msg::NodeScan>("/node_scans", rclcpp::QoS(2000).reliable());
        graph_timer_ = create_wall_timer(std::chrono::seconds(2), std::bind(&GraphBuilderNode::publish_graph, this));
        tf_timer_ = create_wall_timer(std::chrono::milliseconds(50), std::bind(&GraphBuilderNode::publish_tf, this));
        RCLCPP_INFO(get_logger(), "GraphBuilder: %s + %s | node every %.2f m / %.2f rad | seq ICP %s | LC search %.1f m, window +-%.2f m +-%.2f rad | noise %s",
                    scan_topic_.c_str(), odom_topic_.c_str(), min_dist_, min_rot_, seq_on_ ? "on" : "off",
                    lc_dist_, lc_win_xy_, lc_win_th_, noise_on_ ? "ON" : "off");
    }

private:
    // ---------------- odometry ----------------
    struct OdomSample { double t; Pose2 p, m; };     // p = used (maybe noisy), m = as in the message

    void odom_cb(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (base_frame_.empty()) base_frame_ = msg->child_frame_id;
        odom_frame_ = msg->header.frame_id;
        const double t = rclcpp::Time(msg->header.stamp).seconds();
        Pose2 p{msg->pose.pose.position.x, msg->pose.pose.position.y, tf2::getYaw(msg->pose.pose.orientation)};
        const Pose2 m = p;
        if (noise_on_) {
            // perturb the INCREMENT since the last message and integrate it (drifting odometry)
            if (!have_true_) { true_prev_ = p; noisy_ = p; have_true_ = true; }
            const Pose2 d = between(true_prev_, p); true_prev_ = p;
            const double dd = std::hypot(d.x, d.y), da = std::abs(d.th);
            std::normal_distribution<double> nt(0.0, std::sqrt(a1_ * dd * dd + a2_ * da * da) + 1e-12),
                                             nr(0.0, std::sqrt(a3_ * dd * dd + a4_ * da * da) + 1e-12);
            const double et = nt(rng_);
            noisy_ = noisy_ * Pose2{d.x + et * (dd > 0 ? d.x / dd : 0), d.y + et * (dd > 0 ? d.y / dd : 0), d.th + nr(rng_)};
            p = noisy_;
        }
        if (!odom_.empty() && t <= odom_.back().t) return;
        odom_.push_back({t, p, m});
        while (odom_.size() > 2000) odom_.pop_front();
        process_pending();
    }

    bool odom_at(double t, Pose2& out, bool message_pose = false) const
    {
        if (odom_.empty() || t < odom_.front().t || t > odom_.back().t) return false;
        auto it = std::lower_bound(odom_.begin(), odom_.end(), t, [](const OdomSample& s, double v) { return s.t < v; });
        const Pose2 OdomSample::* f_ = message_pose ? &OdomSample::m : &OdomSample::p;
        if (it == odom_.begin()) { out = (*it).*f_; return true; }
        const Pose2& b = (*it).*f_; const Pose2& a = (*(it - 1)).*f_;
        const double f = (t - (it - 1)->t) / std::max(it->t - (it - 1)->t, 1e-9);
        out = {a.x + f * (b.x - a.x), a.y + f * (b.y - a.y), wrap(a.th + f * wrap(b.th - a.th))};
        return true;
    }

    // ---------------- scans -> nodes ----------------
    void scan_cb(const sensor_msgs::msg::LaserScan::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_.push_back(msg);
        while (pending_.size() > 20) pending_.pop_front();
        process_pending();
    }

    // a scan is processed once the odometry covers its last beam
    void process_pending()
    {
        while (!pending_.empty()) {
            const auto& s = *pending_.front();
            const size_t n = s.ranges.size();
            const double dt = (deskew_ && s.time_increment > 0.f) ? s.time_increment : 0.0;
            const double stamp = rclcpp::Time(s.header.stamp).seconds();
            const double t_first = stamp_end_ ? stamp - dt * (n - 1) : stamp, t_last = t_first + dt * (n - 1);
            if (odom_.empty() || odom_.back().t < t_last) return;                 // wait for odometry
            auto msg = pending_.front(); pending_.pop_front();
            Pose2 o_ref;
            if (!odom_at(t_last, o_ref)) continue;                                // older than the odometry buffer
            if (!nodes_.empty()) {
                const Pose2 d = between(nodes_.back().odom, o_ref);
                if (std::hypot(d.x, d.y) < min_dist_ && std::abs(d.th) < min_rot_) continue;
            }
            Pose2 lidar;
            if (!lidar_pose(msg->header.frame_id, lidar)) continue;
            Cloud c;
            std::vector<float> polar(n, 0.f);
            for (size_t i = 0; i < n; ++i) {
                const float rr = msg->ranges[i];
                if (std::isfinite(rr) && rr >= std::max<double>(msg->range_min, min_range_) && rr < msg->range_max) {
                    const int b = beam_index(msg->angle_min + i * msg->angle_increment, n);
                    if (polar[b] == 0.f || rr < polar[b]) polar[b] = rr;
                }
                const float r = msg->ranges[i];
                if (!std::isfinite(r) || r < std::max<double>(msg->range_min, min_range_) || r > std::min<double>(msg->range_max, max_range_)) continue;
                const double a = msg->angle_min + i * msg->angle_increment;
                Vec2 p = lidar.apply({r * std::cos(a), r * std::sin(a)});     // base frame at the beam's time
                if (dt > 0) { Pose2 oi; if (odom_at(t_first + dt * i, oi)) p = between(o_ref, oi).apply(p); }   // -> base frame at t_last
                c.p.push_back(p);
            }
            filter_moved(c, o_ref, msg->header.stamp);
            sm::compute_normals(c);
            Pose2 o_msg; odom_at(t_last, o_msg, true);
            add_node(t_last, o_ref, o_msg, std::move(c), std::move(polar), msg->header.stamp);
        }
    }

    static int beam_index(double angle, size_t n)
    {
        return static_cast<int>(std::floor((wrap(angle) + M_PI) / (2 * M_PI) * n)) % static_cast<int>(n);
    }

    // beams b-w .. b+w of a polar scan: 1 = looked past `range`, 0 = something at or before it, -1 = too few returns to say.
    // An empty beam is no evidence: real lidars have NaN beams.
    static int looked_past(const std::vector<float>& img, int b, int w, double range)
    {
        const int N = static_cast<int>(img.size());
        int seen = 0;
        for (int d = -w; d <= w; ++d) {
            const float v = img[(b + d + N) % N];
            if (v == 0.f) continue;
            if (v <= range) return 0;
            ++seen;
        }
        return 2 * seen >= 2 * w + 1 ? 1 : -1;
    }

    // drop points in space an older scan looked through (see the header)
    void filter_moved(Cloud& c, const Pose2& odom, const builtin_interfaces::msg::Time& stamp)
    {
        if (dyn_back_.empty() || nodes_.empty()) return;
        const GraphNode& last = nodes_.back();
        const Pose2 step = between(last.odom, odom), cur = last.est * step, chain = last.chain * step, lidar_inv = lidar_.inverse();
        std::vector<char> keep(c.size(), 1);
        for (const int64_t back : dyn_back_) {
            const int k = static_cast<int>(nodes_.size()) - static_cast<int>(back);
            if (k < 0 || nodes_[k].polar.empty()) continue;
            const Pose2 T = lidar_inv * nodes_[k].chain.inverse() * chain;                       // cur base -> old lidar
            const auto& img = nodes_[k].polar; const int N = static_cast<int>(img.size());
            for (size_t i = 0; i < c.size(); ++i) {
                if (!keep[i]) continue;
                const Vec2 q = T.apply(c.p[i]); const double r = q.norm();
                const int b = beam_index(std::atan2(q.y(), q.x()), N);
                if (looked_past(img, b, dyn_tol_, r + dyn_margin_) == 1) { keep[i] = 0; ++n_dyn_; }
            }
        }
        std::vector<float> moved;
        size_t w = 0;
        for (size_t i = 0; i < c.size(); ++i) {
            if (keep[i]) { c.p[w++] = c.p[i]; continue; }
            const Vec2 m = cur.apply(c.p[i]);
            moved.push_back(static_cast<float>(m.x())); moved.push_back(static_cast<float>(m.y())); moved.push_back(0.f);
        }
        c.p.resize(w);
        publish_moved(moved, stamp);
    }

    void publish_moved(const std::vector<float>& xyz, const builtin_interfaces::msg::Time& stamp)
    {
        sensor_msgs::msg::PointCloud2 cl;
        cl.header.stamp = stamp; cl.header.frame_id = map_frame_;
        cl.height = 1; cl.width = static_cast<uint32_t>(xyz.size() / 3);
        cl.is_bigendian = false; cl.is_dense = true;
        cl.fields.resize(3);
        const char* names[3] = {"x", "y", "z"};
        for (int k = 0; k < 3; ++k) {
            cl.fields[k].name = names[k]; cl.fields[k].offset = 4 * k;
            cl.fields[k].datatype = sensor_msgs::msg::PointField::FLOAT32; cl.fields[k].count = 1;
        }
        cl.point_step = 12; cl.row_step = cl.point_step * cl.width;
        cl.data.resize(cl.row_step);
        if (!xyz.empty()) std::memcpy(cl.data.data(), xyz.data(), cl.row_step);
        moved_pub_->publish(cl);
    }

    bool lidar_pose(const std::string& frame, Pose2& out)
    {
        if (have_lidar_) { out = lidar_; return true; }
        try {
            auto tf = tf_buffer_->lookupTransform(base_frame_, frame, tf2::TimePointZero);
            lidar_ = {tf.transform.translation.x, tf.transform.translation.y, tf2::getYaw(tf.transform.rotation)};
            have_lidar_ = true; out = lidar_;
            RCLCPP_INFO(get_logger(), "lidar %s in %s: x %.3f y %.3f yaw %.3f", frame.c_str(), base_frame_.c_str(), lidar_.x, lidar_.y, lidar_.th);
            return true;
        } catch (const tf2::TransformException& e) {
            RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000, "waiting for TF %s -> %s: %s", base_frame_.c_str(), frame.c_str(), e.what());
            return false;
        }
    }

    // ---------------- edges ----------------
    void push_edge(int i, int j, const Pose2& z, const Eigen::Matrix3d& omega, const char* type)
    {
        slam_msgs::msg::PoseEdge e;
        e.from_id = i; e.to_id = j; e.edge_type = type;
        e.z = {z.x, z.y, z.th, 0, 0, 0};
        for (int r = 0; r < 3; ++r) for (int c = 0; c < 3; ++c) e.omega[3 * r + c] = omega(r, c);
        edges_.push_back(e);
    }

    Eigen::Matrix3d odom_info(const Pose2& z) const
    {
        const double d = std::hypot(z.x, z.y), a = std::abs(z.th);
        const double vt = kt_ * kt_ * d * d + ktr_ * ktr_ * a * a + ft_ * ft_;
        const double vr = krt_ * krt_ * d * d + kr_ * kr_ * a * a + fr_ * fr_;
        return Eigen::Vector3d(1 / vt, 1 / vt, 1 / vr).asDiagonal();
    }

    Eigen::Matrix3d icp_info(const sm::IcpResult& r) const
    {
        const double s = std::max(r.rms, icp_floor_);
        return icp_scale_ * r.H / (s * s);
    }

    // reference cloud: nodes [a, b] expressed in node `frame`'s coordinates (current estimates)
    Cloud local_map(int a, int b, int frame) const
    {
        Cloud c; const Pose2 inv = nodes_[frame].est.inverse();
        for (int k = std::max(0, a); k <= std::min(b, static_cast<int>(nodes_.size()) - 1); ++k) {
            const Pose2 T = inv * nodes_[k].est;
            const Eigen::Matrix2d R = Eigen::Rotation2Dd(T.th).toRotationMatrix();
            for (size_t i = 0; i < nodes_[k].scan.size(); ++i) {
                c.p.push_back(T.apply(nodes_[k].scan.p[i]));
                c.n.push_back(R * nodes_[k].scan.n[i]); c.has_n.push_back(nodes_[k].scan.has_n[i]);
            }
        }
        return c;
    }

    void add_node(double t, const Pose2& odom, const Pose2& odom_msg, Cloud&& scan, std::vector<float>&& polar, const builtin_interfaces::msg::Time& stamp)
    {
        GraphNode nd; nd.id = static_cast<int>(nodes_.size()); nd.t = t; nd.odom = odom; nd.odom_msg = odom_msg; nd.scan = std::move(scan); nd.polar = std::move(polar);
        if (nodes_.empty()) {
            // map frame = odom frame at the start: node 0 sits at its odometry pose, NOT at the origin
            // (the first processed scan can come after the robot already moved - found 2026-10-18:
            // a 15 deg turn before node 0 rotated the whole map by 15 deg)
            nd.est = nd.chain = nd.odom;
            nodes_.push_back(std::move(nd));
            RCLCPP_INFO(get_logger(), "Node 0 at its odometry pose (%.3f, %.3f, %.3f), %zu points.",
                        nodes_[0].est.x, nodes_[0].est.y, nodes_[0].est.th, nodes_[0].scan.size());
            publish_node_scan(nodes_[0], stamp);
            return;
        }
        const GraphNode& prev = nodes_.back();
        const Pose2 z_odom = between(prev.odom, odom);
        push_edge(prev.id, nd.id, z_odom, odom_info(z_odom), "odometry");
        nd.seq_z = z_odom;
        if (seq_on_) {
            const Cloud ref = local_map(prev.id - seq_nodes_ + 1, prev.id, prev.id);
            const sm::IcpResult r = sm::pl_icp(ref, nd.scan, z_odom);
            const Pose2 d = between(z_odom, r.T);
            if (r.ok && r.overlap >= seq_overlap_ && r.rms <= seq_rms_ && std::hypot(d.x, d.y) <= seq_disagree_ && std::abs(d.th) <= seq_disagree_) {
                push_edge(prev.id, nd.id, r.T, icp_info(r), "scan_match");
                nd.seq_z = r.T; ++n_seq_ok_;
            } else {
                ++n_seq_bad_;
                RCLCPP_DEBUG(get_logger(), "seq ICP %d->%d rejected: ok %d overlap %.2f rms %.4f", prev.id, nd.id, r.ok, r.overlap, r.rms);
            }
        }
        nd.est = prev.est * nd.seq_z; nd.chain = prev.chain * nd.seq_z;
        nd.travel = prev.travel + std::hypot(z_odom.x, z_odom.y);
        nodes_.push_back(std::move(nd));
        publish_node_scan(nodes_.back(), stamp);
        check_loop_closure(static_cast<int>(nodes_.size()) - 1);
    }

    void check_loop_closure(int cur)
    {
        if (cur < lc_min_gap_ || cur - last_lc_ < lc_cooldown_) return;
        const GraphNode& c = nodes_[cur];
        std::vector<std::pair<double, int>> cands;
        for (int i = 0; i <= cur - lc_min_gap_; ++i) {
            if (c.travel - nodes_[i].travel < lc_min_travel_) continue;
            const double d = std::hypot(nodes_[i].est.x - c.est.x, nodes_[i].est.y - c.est.y);
            if (d <= lc_dist_) cands.push_back({d, i});
        }
        std::sort(cands.begin(), cands.end());
        std::vector<int> tried;
        for (const auto& [dist, i] : cands) {
            if (static_cast<int>(tried.size()) >= lc_cands_) break;
            bool near_tried = false;                                     // candidates far apart along the path
            for (int k : tried) near_tried |= std::abs(k - i) <= 2 * lc_submap_;
            if (near_tried) continue;
            tried.push_back(i);
            const Cloud ref = local_map(i - lc_submap_, i + lc_submap_, i);
            const Pose2 guess = between(nodes_[i].est, c.est);
            double score;
            const Pose2 coarse = sm::correlative(ref, c.scan, guess, lc_win_xy_, lc_win_th_, score);
            if (score < lc_score_) { RCLCPP_DEBUG(get_logger(), "LC %d<->%d score %.2f rejected", i, cur, score); continue; }
            const sm::IcpResult r = sm::pl_icp(ref, c.scan, coarse);
            if (!r.ok || r.overlap < lc_overlap_ || r.rms > lc_rms_) {
                RCLCPP_INFO(get_logger(), "LC %d<->%d REJECTED: score %.2f overlap %.2f rms %.4f", i, cur, score, r.overlap, r.rms);
                continue;
            }
            push_edge(i, cur, r.T, icp_info(r), "loop_closure");
            last_lc_ = cur; ++n_lc_;
            publish_graph_locked();
            const Pose2 corr = between(guess, r.T);
            RCLCPP_INFO(get_logger(), "LC %d<->%d accepted: dist %.2f score %.2f overlap %.2f rms %.4f | correction %.3f m %.3f rad",
                        i, cur, dist, score, r.overlap, r.rms, std::hypot(corr.x, corr.y), corr.th);
            return;
        }
    }

    // ---------------- back end interface ----------------
    void path_cb(const nav_msgs::msg::Path::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const size_t n = std::min(msg->poses.size(), nodes_.size());
        for (size_t i = 0; i < n; ++i) {
            const auto& p = msg->poses[i].pose;
            nodes_[i].est = {p.position.x, p.position.y, tf2::getYaw(p.orientation)};
        }
        for (size_t i = std::max<size_t>(n, 1); i < nodes_.size(); ++i) nodes_[i].est = nodes_[i - 1].est * nodes_[i].seq_z;
    }

    void publish_graph()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        publish_graph_locked();
    }

    // caller holds mutex_. Also called right after a loop closure is accepted, so the back end corrects the
    // map at once instead of up to 2 s later.
    void publish_graph_locked()
    {
        if (nodes_.size() < 2) return;
        slam_msgs::msg::PoseGraph g;
        g.header.stamp = now(); g.header.frame_id = map_frame_;
        g.num_nodes = static_cast<int>(nodes_.size());
        for (const auto& n : nodes_) { g.node_poses.push_back(n.est.x); g.node_poses.push_back(n.est.y); g.node_poses.push_back(n.est.th); }
        g.edges = edges_;
        graph_pub_->publish(g);
        // odometry alone (what the graph would be without scan matching), in the map frame (= odom at the start)
        nav_msgs::msg::Path op; op.header = g.header;
        for (const auto& n : nodes_) {
            const Pose2 p = n.odom; geometry_msgs::msg::PoseStamped ps; ps.header = g.header;
            ps.pose.position.x = p.x; ps.pose.position.y = p.y; ps.pose.orientation.z = std::sin(p.th / 2); ps.pose.orientation.w = std::cos(p.th / 2);
            op.poses.push_back(ps);
        }
        odom_path_pub_->publish(op);
        RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 10000, "Graph: %zu nodes %zu edges | seq ICP ok %d rejected %d | LC %d | moved points dropped %ld",
                             nodes_.size(), edges_.size(), n_seq_ok_, n_seq_bad_, n_lc_, n_dyn_);
    }

    void publish_node_scan(const GraphNode& n, const builtin_interfaces::msg::Time& stamp)
    {
        slam_msgs::msg::NodeScan m;
        m.header.stamp = stamp; m.header.frame_id = base_frame_; m.id = n.id;
        for (const auto& p : n.scan.p) { m.x.push_back(static_cast<float>(p.x())); m.y.push_back(static_cast<float>(p.y())); }
        node_scan_pub_->publish(m);
    }

    void publish_tf()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!publish_tf_ || nodes_.empty() || odom_frame_.empty()) return;
        const Pose2 T = nodes_.back().est * nodes_.back().odom_msg.inverse();    // map <- odom (the /odom frame the TF tree uses)
        geometry_msgs::msg::TransformStamped tf;
        tf.header.stamp = now(); tf.header.frame_id = map_frame_; tf.child_frame_id = odom_frame_;
        tf.transform.translation.x = T.x; tf.transform.translation.y = T.y;
        tf.transform.rotation.z = std::sin(T.th / 2); tf.transform.rotation.w = std::cos(T.th / 2);
        tf_pub_->sendTransform(tf);
    }

    std::string odom_topic_, scan_topic_, map_frame_, base_frame_, odom_frame_;
    bool publish_tf_, stamp_end_, deskew_, seq_on_, noise_on_;
    double min_range_, max_range_, min_dist_, min_rot_, kt_, ktr_, krt_, kr_, ft_, fr_;
    double seq_overlap_, seq_rms_, seq_disagree_, icp_floor_, icp_scale_;
    double lc_dist_, lc_win_xy_, lc_win_th_, lc_score_, lc_overlap_, lc_rms_, lc_min_travel_;
    double a1_, a2_, a3_, a4_;
    int seq_nodes_, lc_min_gap_, lc_cooldown_, lc_cands_, lc_submap_;
    int last_lc_ = -1000, n_seq_ok_ = 0, n_seq_bad_ = 0, n_lc_ = 0;
    std::vector<int64_t> dyn_back_; double dyn_margin_; int dyn_tol_; long n_dyn_ = 0;
    std::mt19937 rng_;
    bool have_true_ = false; Pose2 true_prev_, noisy_;
    bool have_lidar_ = false; Pose2 lidar_;

    std::mutex mutex_;
    std::deque<OdomSample> odom_;
    std::deque<sensor_msgs::msg::LaserScan::SharedPtr> pending_;
    std::vector<GraphNode> nodes_;
    std::vector<slam_msgs::msg::PoseEdge> edges_;

    std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_pub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr path_sub_;
    rclcpp::Publisher<slam_msgs::msg::PoseGraph>::SharedPtr graph_pub_;
    rclcpp::Publisher<slam_msgs::msg::NodeScan>::SharedPtr node_scan_pub_;
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr odom_path_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr moved_pub_;
    rclcpp::TimerBase::SharedPtr graph_timer_, tf_timer_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GraphBuilderNode>());
    rclcpp::shutdown();
    return 0;
}
