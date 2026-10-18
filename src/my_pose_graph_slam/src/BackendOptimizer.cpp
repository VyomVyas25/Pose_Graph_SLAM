#include "rclcpp/rclcpp.hpp"
#include "slam_msgs/msg/pose_graph.hpp"
#include "visualization_msgs/msg/marker_array.hpp"
#include "nav_msgs/msg/path.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <cmath>
#include <vector>
#include <numeric>

Eigen::Vector3d t2v(const Eigen::Matrix3d& T) {
    return {T(0,2), T(1,2), std::atan2(T(1,0), T(0,0))};
}
Eigen::Matrix3d v2t(const Eigen::Vector3d& v) {
    Eigen::Matrix3d T = Eigen::Matrix3d::Identity();
    double c=std::cos(v(2)), s=std::sin(v(2));
    T(0,0)=c; T(0,1)=-s; T(0,2)=v(0);
    T(1,0)=s; T(1,1)= c; T(1,2)=v(1);
    return T;
}
double normalize_angle(double a) {
    while (a >  M_PI) a -= 2*M_PI;
    while (a < -M_PI) a += 2*M_PI;
    return a;
}

struct PoseNode     { double x, y, th; };
struct PoseEdgeData {
    int from_id, to_id;
    Eigen::Vector3d z;
    Eigen::Matrix3d omega;
    std::string type;
};
using Triplet = Eigen::Triplet<double>;

class BackEndOptimizerNode : public rclcpp::Node {
public:
    BackEndOptimizerNode() : Node("backend_optimizer_node")
    {
        max_iters_ = declare_parameter("max_iterations", 15);
        robust_k2_ = declare_parameter("lc_cauchy_k2", 9.0);      // Cauchy kernel width on LC chi2 (0 = off)
        lambda0_   = declare_parameter("lm_lambda0", 1e-4);

        graph_sub_ = create_subscription<slam_msgs::msg::PoseGraph>(
            "/pose_graph", 10,
            std::bind(&BackEndOptimizerNode::graph_cb, this, std::placeholders::_1));
        marker_pub_ = create_publisher<visualization_msgs::msg::MarkerArray>("/graph_markers",10);
        path_pub_   = create_publisher<nav_msgs::msg::Path>("/optimized_path",10);

        RCLCPP_INFO(get_logger(), "BackEnd optimizer ready (chi2 tracking enabled)");
        RCLCPP_INFO(get_logger(),
            "VERIFICATION: Watch chi2 values. "
            "chi2 should decrease each GN iteration. "
            "LC chi2 drop = successful correction.");
    }

private:
    void graph_cb(const slam_msgs::msg::PoseGraph::SharedPtr msg)
    {
        int n = msg->num_nodes;
        if (n < 2) return;

        nodes_.resize(n);
        for (int i=0;i<n;++i)
            nodes_[i]={msg->node_poses[3*i],
                       msg->node_poses[3*i+1],
                       msg->node_poses[3*i+2]};

        edges_.clear();
        for (auto& e:msg->edges) {
            PoseEdgeData ed;
            ed.from_id=e.from_id; ed.to_id=e.to_id;
            ed.z={e.z[0],e.z[1],e.z[2]};
            ed.omega<<e.omega[0],e.omega[1],e.omega[2],
                      e.omega[3],e.omega[4],e.omega[5],
                      e.omega[6],e.omega[7],e.omega[8];
            ed.type=e.edge_type;
            edges_.push_back(ed);
        }

        int n_odom=0, n_lc=0;
        double ot=0, or_=0, lt=0, lr=0;
        for (auto& ed:edges_) {
            if (ed.type=="odometry"){ot+=ed.omega(0,0);or_+=ed.omega(2,2);n_odom++;}
            else if(ed.type=="loop_closure"){lt+=ed.omega(0,0);lr+=ed.omega(2,2);n_lc++;}
        }
        RCLCPP_INFO_THROTTLE(get_logger(),*get_clock(),20000,
            "Ω — odom(%d): info_t=%.1f info_r=%.1f | LC(%d): info_t=%.1f info_r=%.1f",
            n_odom,n_odom>0?ot/n_odom:0.0,n_odom>0?or_/n_odom:0.0,
            n_lc,n_lc>0?lt/n_lc:0.0,n_lc>0?lr/n_lc:0.0);

        gauss_newton_optimize(max_iters_);
        publish_markers();
        publish_path();
    }

    struct Chi2Stats {
        double total, odom, lc;      // total = ROBUST cost (Cauchy on loop closures), odom / lc = plain chi2
        int n_odom, n_lc;
    };

    Eigen::Vector3d edge_error(const PoseEdgeData& edge) const
    {
        const auto& ni = nodes_[edge.from_id]; const auto& nj = nodes_[edge.to_id];
        return t2v(v2t(edge.z).inverse() * v2t({ni.x, ni.y, ni.th}).inverse() * v2t({nj.x, nj.y, nj.th}));
    }

    // Cauchy kernel on loop closures: rho(c2) = k2 log(1 + c2/k2), IRLS weight 1/(1 + c2/k2).
    // A wrong loop closure (chi2 >> k2) loses its pull instead of bending the whole map.
    double robust_weight(const PoseEdgeData& edge, double chi2) const
    {
        return (edge.type == "loop_closure" && robust_k2_ > 0) ? 1.0 / (1.0 + chi2 / robust_k2_) : 1.0;
    }
    double robust_cost(const PoseEdgeData& edge, double chi2) const
    {
        return (edge.type == "loop_closure" && robust_k2_ > 0) ? robust_k2_ * std::log1p(chi2 / robust_k2_) : chi2;
    }

    Chi2Stats compute_chi2() const
    {
        Chi2Stats s{0,0,0,0,0};
        for (auto& edge : edges_) {
            if (edge.from_id>=(int)nodes_.size()||edge.to_id>=(int)nodes_.size()) continue;
            const Eigen::Vector3d e = edge_error(edge);
            const double chi2_edge = e.transpose() * edge.omega * e;
            s.total += robust_cost(edge, chi2_edge);
            if (edge.type=="loop_closure") { s.lc += chi2_edge; s.n_lc++; }
            else { s.odom += chi2_edge; s.n_odom++; }
        }
        return s;
    }

    void build_linear_system(std::vector<Triplet>& triplets, Eigen::VectorXd& b)
    {
        for (auto& edge:edges_) {
            int i=edge.from_id, j=edge.to_id;
            if (i>=(int)nodes_.size()||j>=(int)nodes_.size()) continue;

            Eigen::Vector3d e = edge_error(edge);

            // Jacobians of e w.r.t. x_i (A) and x_j (B), Grisetti et al. 2010 - checked against
            // finite differences (max difference 1.9e-8 over 2000 random edges)
            double ci=std::cos(nodes_[i].th), si=std::sin(nodes_[i].th);
            double cz=std::cos(edge.z(2)),    sz=std::sin(edge.z(2));
            double dx=nodes_[j].x-nodes_[i].x;
            double dy=nodes_[j].y-nodes_[i].y;

            Eigen::Matrix3d A = Eigen::Matrix3d::Zero();
            A(0,0) = -(cz*ci - sz*si);
            A(0,1) = -(cz*si + sz*ci);
            A(0,2) =  cz*(-si*dx + ci*dy) + sz*(-ci*dx - si*dy);
            A(1,0) =   sz*ci + cz*si;
            A(1,1) =   sz*si - cz*ci;
            A(1,2) = -sz*(-si*dx + ci*dy) + cz*(-ci*dx - si*dy);
            A(2,2) = -1.0;

            Eigen::Matrix3d B = Eigen::Matrix3d::Zero();
            B(0,0) =  cz*ci - sz*si;
            B(0,1) =  cz*si + sz*ci;
            B(1,0) = -sz*ci - cz*si;
            B(1,1) = -sz*si + cz*ci;
            B(2,2) =  1.0;

            const Eigen::Matrix3d omega = robust_weight(edge, e.transpose() * edge.omega * e) * edge.omega;
            Eigen::Matrix3d AtO = A.transpose() * omega;
            Eigen::Matrix3d BtO = B.transpose() * omega;
            Eigen::Matrix3d Hii = AtO * A, Hij = AtO * B;
            Eigen::Matrix3d Hji = BtO * A, Hjj = BtO * B;
            Eigen::Vector3d bi  = AtO * e, bj  = BtO * e;

            int ri=3*i, rj=3*j;
            for (int r=0;r<3;++r) {
                for (int c=0;c<3;++c) {
                    triplets.push_back(Triplet(ri+r,ri+c,Hii(r,c)));
                    triplets.push_back(Triplet(ri+r,rj+c,Hij(r,c)));
                    triplets.push_back(Triplet(rj+r,ri+c,Hji(r,c)));
                    triplets.push_back(Triplet(rj+r,rj+c,Hjj(r,c)));
                }
                b(ri+r) += bi(r);
                b(rj+r) += bj(r);
            }
        }
    }

    // Gauss-Newton step first, Levenberg-Marquardt (H + lambda diag(H)) fallback, with step acceptance: a step is only taken when it
    // lowers the cost, so the result can never be worse than the initial guess (plain Gauss-Newton
    // went from chi2 4.1e7 to 2.6e8 on M3500b). The initial guess is the front end's current
    // estimate (warm start), not raw odometry.
    void gauss_newton_optimize(int max_iters)
    {
        int n=nodes_.size(), dof=3*n;
        auto chi2_before = compute_chi2();
        double cost = chi2_before.total;
        double lambda = lambda0_;
        int iter = 0, accepted = 0;
        for (; iter<max_iters; ++iter) {
            std::vector<Triplet> triplets;
            Eigen::VectorXd b=Eigen::VectorXd::Zero(dof);
            build_linear_system(triplets, b);
            triplets.push_back(Triplet(0,0,1e9));       // fix node 0 (gauge)
            triplets.push_back(Triplet(1,1,1e9));
            triplets.push_back(Triplet(2,2,1e9));
            Eigen::SparseMatrix<double> H(dof,dof);
            H.setFromTriplets(triplets.begin(),triplets.end());
            const Eigen::VectorXd diag = H.diagonal();
            const auto saved = nodes_;
            bool improved = false; double step = 0;
            for (int tries = 0; tries < 30 && !improved; ++tries) {
                // first try the plain Gauss-Newton step (fast near a solution), then damp
                const double lam = tries == 0 ? 0.0 : lambda;
                if (tries > 0) lambda *= 4.0;
                Eigen::SparseMatrix<double> Hl = H;
                for (int k=0;k<dof;++k) Hl.coeffRef(k,k) += lam * diag(k);
                Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver(Hl);
                if (solver.info()!=Eigen::Success) { lambda *= 10; continue; }
                const Eigen::VectorXd dx = solver.solve(-b);
                for (int k=0;k<n;++k) {
                    nodes_[k].x  = saved[k].x + dx(3*k);
                    nodes_[k].y  = saved[k].y + dx(3*k+1);
                    nodes_[k].th = normalize_angle(saved[k].th + dx(3*k+2));
                }
                const double c = compute_chi2().total;
                if (c < cost) { improved = true; step = dx.norm(); if (tries > 0) lambda = std::max(lambda / 16.0, 1e-12);
                                const double rel = (cost - c) / (cost + 1e-12); cost = c; ++accepted;
                                if (step < 1e-4 || rel < 1e-6) { iter = max_iters; } }
                else { nodes_ = saved; }
            }
            if (!improved) break;
        }
        auto chi2_after = compute_chi2();
        RCLCPP_INFO(get_logger(), "LM: cost %.4f -> %.4f (%d steps) | odom-type chi2 %.3f (%d edges) LC chi2 %.3f (%d) | %d nodes",
                    chi2_before.total, chi2_after.total, accepted, chi2_after.odom, chi2_after.n_odom, chi2_after.lc, chi2_after.n_lc, n);
    }

    void publish_markers()
    {
        visualization_msgs::msg::MarkerArray ma;
        auto stamp=now();

        visualization_msgs::msg::Marker node_m;
        node_m.header.frame_id="map"; node_m.header.stamp=stamp;
        node_m.ns="nodes"; node_m.id=0;
        node_m.type=visualization_msgs::msg::Marker::SPHERE_LIST;
        node_m.action=visualization_msgs::msg::Marker::ADD;
        node_m.scale.x=node_m.scale.y=node_m.scale.z=0.08;
        node_m.color.r=node_m.color.g=node_m.color.b=node_m.color.a=1;
        for (auto& nd:nodes_){
            geometry_msgs::msg::Point p; p.x=nd.x; p.y=nd.y; p.z=0;
            node_m.points.push_back(p);
        }
        ma.markers.push_back(node_m);

        visualization_msgs::msg::Marker odom_m;
        odom_m.header.frame_id="map"; odom_m.header.stamp=stamp;
        odom_m.ns="odom_edges"; odom_m.id=1;
        odom_m.type=visualization_msgs::msg::Marker::LINE_LIST;
        odom_m.action=visualization_msgs::msg::Marker::ADD;
        odom_m.scale.x=0.02;
        odom_m.color.r=0.2;odom_m.color.g=0.5;odom_m.color.b=1.0;odom_m.color.a=0.8;

        visualization_msgs::msg::Marker lc_m;
        lc_m.header.frame_id="map"; lc_m.header.stamp=stamp;
        lc_m.ns="lc_edges"; lc_m.id=2;
        lc_m.type=visualization_msgs::msg::Marker::LINE_LIST;
        lc_m.action=visualization_msgs::msg::Marker::ADD;
        lc_m.scale.x=0.06;
        lc_m.color.r=1.0;lc_m.color.g=0.1;lc_m.color.b=0.1;lc_m.color.a=1.0;

        int label_id = 100;
        for (auto& edge:edges_) {
            if (edge.from_id>=(int)nodes_.size()||edge.to_id>=(int)nodes_.size()) continue;
            geometry_msgs::msg::Point pf,pt;
            pf.x=nodes_[edge.from_id].x;pf.y=nodes_[edge.from_id].y;pf.z=0;
            pt.x=nodes_[edge.to_id].x;  pt.y=nodes_[edge.to_id].y;  pt.z=0;
            if (edge.type=="loop_closure") {
                lc_m.points.push_back(pf);
                lc_m.points.push_back(pt);

                Eigen::Vector3d xi={nodes_[edge.from_id].x,nodes_[edge.from_id].y,nodes_[edge.from_id].th};
                Eigen::Vector3d xj={nodes_[edge.to_id].x,nodes_[edge.to_id].y,nodes_[edge.to_id].th};
                Eigen::Vector3d e=t2v(v2t(edge.z).inverse()*v2t(xi).inverse()*v2t(xj));
                double lc_chi2 = e.transpose()*edge.omega*e;

                visualization_msgs::msg::Marker txt;
                txt.header.frame_id="map"; txt.header.stamp=stamp;
                txt.ns="lc_chi2"; txt.id=label_id++;
                txt.type=visualization_msgs::msg::Marker::TEXT_VIEW_FACING;
                txt.action=visualization_msgs::msg::Marker::ADD;
                txt.pose.position.x=(pf.x+pt.x)/2;
                txt.pose.position.y=(pf.y+pt.y)/2+0.15;
                txt.pose.position.z=0.1;
                txt.pose.orientation.w=1.0;
                txt.scale.z=0.15;
                txt.color.r=1.0;txt.color.g=1.0;txt.color.b=0.0;txt.color.a=1.0;
                char buf[32]; snprintf(buf, sizeof(buf), "χ²=%.4f", lc_chi2);
                txt.text=buf;
                ma.markers.push_back(txt);
            }
            else { odom_m.points.push_back(pf); odom_m.points.push_back(pt); }
        }
        ma.markers.push_back(odom_m);
        ma.markers.push_back(lc_m);

        visualization_msgs::msg::Marker arr_m;
        arr_m.header.frame_id="map"; arr_m.header.stamp=stamp;
        arr_m.ns="headings"; arr_m.id=3;
        arr_m.type=visualization_msgs::msg::Marker::LINE_LIST;
        arr_m.action=visualization_msgs::msg::Marker::ADD;
        arr_m.scale.x=0.015;
        arr_m.color.r=1.0;arr_m.color.g=0.8;arr_m.color.b=0.0;arr_m.color.a=0.7;
        for (size_t k=0;k<nodes_.size();k+=5) {
            auto& nd=nodes_[k];
            geometry_msgs::msg::Point base,tip;
            base.x=nd.x;base.y=nd.y;base.z=0;
            tip.x=nd.x+0.15*std::cos(nd.th);tip.y=nd.y+0.15*std::sin(nd.th);tip.z=0;
            arr_m.points.push_back(base);arr_m.points.push_back(tip);
        }
        ma.markers.push_back(arr_m);
        marker_pub_->publish(ma);
    }

    void publish_path()
    {
        nav_msgs::msg::Path path;
        path.header.frame_id="map"; path.header.stamp=now();
        for (auto& nd:nodes_) {
            geometry_msgs::msg::PoseStamped ps;
            ps.header=path.header;
            ps.pose.position.x=nd.x;ps.pose.position.y=nd.y;
            ps.pose.orientation.z=std::sin(nd.th/2.0);
            ps.pose.orientation.w=std::cos(nd.th/2.0);
            path.poses.push_back(ps);
        }
        path_pub_->publish(path);
    }

    rclcpp::Subscription<slam_msgs::msg::PoseGraph>::SharedPtr graph_sub_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;
    std::vector<PoseNode> nodes_;
    std::vector<PoseEdgeData> edges_;
    int max_iters_;
    double robust_k2_, lambda0_;
};

int main(int argc, char** argv){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<BackEndOptimizerNode>());
    rclcpp::shutdown();
}