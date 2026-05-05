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
        double total, odom, lc;
        int n_odom, n_lc;
    };

    Chi2Stats compute_chi2() const
    {
        Chi2Stats s{0,0,0,0,0};
        for (auto& edge : edges_) {
            int i=edge.from_id, j=edge.to_id;
            if (i>=(int)nodes_.size()||j>=(int)nodes_.size()) continue;
            Eigen::Vector3d xi={nodes_[i].x,nodes_[i].y,nodes_[i].th};
            Eigen::Vector3d xj={nodes_[j].x,nodes_[j].y,nodes_[j].th};
            Eigen::Vector3d e = t2v(v2t(edge.z).inverse() * v2t(xi).inverse() * v2t(xj));
            double chi2_edge = e.transpose() * edge.omega * e;
            s.total += chi2_edge;
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

            Eigen::Vector3d xi={nodes_[i].x,nodes_[i].y,nodes_[i].th};
            Eigen::Vector3d xj={nodes_[j].x,nodes_[j].y,nodes_[j].th};

            Eigen::Vector3d e = t2v(v2t(edge.z).inverse() * v2t(xi).inverse() * v2t(xj));

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

            Eigen::Matrix3d AtO = A.transpose() * edge.omega;
            Eigen::Matrix3d BtO = B.transpose() * edge.omega;
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

    void gauss_newton_optimize(int max_iters)
    {
        int n=nodes_.size(), dof=3*n;

        auto chi2_before = compute_chi2();
        bool has_lc = chi2_before.n_lc > 0;

        if (has_lc) {
            RCLCPP_INFO(get_logger(),
                "chi2 BEFORE opt: total=%.6f  odom=%.6f  LC=%.6f  "
                "(n_odom=%d n_lc=%d)",
                chi2_before.total, chi2_before.odom, chi2_before.lc,
                chi2_before.n_odom, chi2_before.n_lc);
        }

        double prev_chi2 = chi2_before.total;

        for (int iter=0;iter<max_iters;++iter) {
            std::vector<Triplet> triplets;
            Eigen::VectorXd b=Eigen::VectorXd::Zero(dof);
            build_linear_system(triplets, b);

            triplets.push_back(Triplet(0,0,1e9));
            triplets.push_back(Triplet(1,1,1e9));
            triplets.push_back(Triplet(2,2,1e9));

            Eigen::SparseMatrix<double> H(dof,dof);
            H.setFromTriplets(triplets.begin(),triplets.end());

            if (iter==0) {
                double sp=100.0*(1.0-(double)H.nonZeros()/((double)H.rows()*H.cols()));
                RCLCPP_INFO_THROTTLE(get_logger(),*get_clock(),20000,
                    "H: %ldx%ld  nnz: %ld  sparsity: %.2f%%",
                    H.rows(),H.cols(),H.nonZeros(),sp);
            }

            Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
            solver.compute(H);
            if (solver.info()!=Eigen::Success) {
                RCLCPP_WARN(get_logger(),"Sparse decomposition failed at iter %d",iter);
                break;
            }
            Eigen::VectorXd dx=solver.solve(-b);
            if (solver.info()!=Eigen::Success) {
                RCLCPP_WARN(get_logger(),"Sparse solve failed at iter %d",iter);
                break;
            }

            for (int k=0;k<n;++k) {
                nodes_[k].x  += dx(3*k);
                nodes_[k].y  += dx(3*k+1);
                nodes_[k].th  = normalize_angle(nodes_[k].th+dx(3*k+2));
            }

            double step=dx.norm();

            if (has_lc) {
                auto chi2_after = compute_chi2();
                double rel_change = (prev_chi2 - chi2_after.total) / (prev_chi2 + 1e-10);
                RCLCPP_INFO(get_logger(),
                    "  iter %d: chi2=%.6f (odom=%.6f LC=%.6f) "
                    "rel_decrease=%.4f  ||Δx||=%.6f",
                    iter, chi2_after.total, chi2_after.odom, chi2_after.lc,
                    rel_change, step);

                if (chi2_after.total > prev_chi2 * 1.1 && iter > 0) {
                    RCLCPP_WARN(get_logger(),
                        "  chi2 INCREASED (%.6f → %.6f) — "
                        "possible numerical issue or inconsistent LC edge",
                        prev_chi2, chi2_after.total);
                }
                prev_chi2 = chi2_after.total;

                if (step < 1e-4 && rel_change < 1e-4) {
                    RCLCPP_INFO(get_logger(),
                        "Converged at iter %d (Δx=%.6f chi2=%.6f LC_chi2=%.6f "
                        "%d nodes %zu edges)",
                        iter, step, chi2_after.total, chi2_after.lc,
                        n, edges_.size());
                    return;
                }
            } else {
                if (step < 1e-4) {
                    RCLCPP_INFO(get_logger(),
                        "Converged at iter %d (Δx=%.6f, %d nodes, %zu edges)",
                        iter, step, n, edges_.size());
                    return;
                }
            }
        }
        RCLCPP_INFO(get_logger(),
            "Max iters reached (%d nodes, %zu edges)", n, edges_.size());
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
};

int main(int argc, char** argv){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<BackEndOptimizerNode>());
    rclcpp::shutdown();
}