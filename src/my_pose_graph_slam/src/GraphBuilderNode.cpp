#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "nav_msgs/msg/path.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "slam_msgs/msg/pose_edge.hpp"
#include "slam_msgs/msg/pose_graph.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <vector>
#include <cmath>
#include <random>
#include <mutex>
#include <algorithm>

using Point2D = Eigen::Vector2d;

struct LineSegment { Point2D start, end; double angle, length; };
struct Corner     { Point2D pos; double descriptor; };
struct PoseNode {
    double x, y, th;
    double raw_x, raw_y, raw_th;
    int    id;
    std::vector<Point2D> scan;
    std::vector<Corner>  corners;
};
struct Edge { slam_msgs::msg::PoseEdge msg; };
struct IcpResult {
    double dx, dy, dth, rms, corr_ratio;
    int iterations; bool converged;
};

static double norm_angle(double a){
    while(a> M_PI) a-=2*M_PI;
    while(a<-M_PI) a+=2*M_PI;
    return a;
}

double point_to_line_dist(const Point2D& p,const Point2D& a,const Point2D& b){
    Point2D ab=b-a; double len=ab.norm();
    if(len<1e-9) return (p-a).norm();
    return (p-(a+std::clamp(ab.dot(p-a)/(len*len),0.0,1.0)*ab)).norm();
}
void split_and_merge(const std::vector<Point2D>& pts,int s,int e,
                     double thr,std::vector<LineSegment>& lines){
    if(e-s<3) return;
    double md=0; int mi=s;
    for(int i=s+1;i<e;++i){double d=point_to_line_dist(pts[i],pts[s],pts[e-1]);if(d>md){md=d;mi=i;}}
    if(md>thr){split_and_merge(pts,s,mi+1,thr,lines);split_and_merge(pts,mi,e,thr,lines);}
    else{LineSegment ls;ls.start=pts[s];ls.end=pts[e-1];
         ls.length=(ls.end-ls.start).norm();
         ls.angle=std::atan2(ls.end.y()-ls.start.y(),ls.end.x()-ls.start.x());
         if(ls.length>0.15) lines.push_back(ls);}
}
bool line_intersection(const LineSegment& l1,const LineSegment& l2,Point2D& pt){
    double x1=l1.start.x(),y1=l1.start.y(),x2=l1.end.x(),y2=l1.end.y();
    double x3=l2.start.x(),y3=l2.start.y(),x4=l2.end.x(),y4=l2.end.y();
    double d=(x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
    if(std::abs(d)<1e-9) return false;
    double t=((x1-x3)*(y3-y4)-(y1-y3)*(x3-x4))/d;
    pt.x()=x1+t*(x2-x1);pt.y()=y1+t*(y2-y1); return true;
}
std::vector<Corner> extract_corners(const std::vector<LineSegment>& lines){
    std::vector<Corner> corners;
    for(size_t i=0;i<lines.size();++i)
        for(size_t j=i+1;j<lines.size();++j){
            double d=std::min({(lines[i].end-lines[j].start).norm(),(lines[i].start-lines[j].end).norm(),
                               (lines[i].end-lines[j].end).norm(),(lines[i].start-lines[j].start).norm()});
            if(d>0.3) continue;
            double ang=std::abs(lines[i].angle-lines[j].angle);
            while(ang>M_PI) ang=std::abs(ang-M_PI);
            if(ang<0.3||ang>2.8) continue;
            Point2D inter; if(!line_intersection(lines[i],lines[j],inter)) continue;
            double ds=std::min({(inter-lines[i].start).norm(),(inter-lines[i].end).norm(),
                                (inter-lines[j].start).norm(),(inter-lines[j].end).norm()});
            if(ds>1.0) continue;
            corners.push_back({inter,ang});
        }
    return corners;
}
std::vector<Corner> extract_features(const std::vector<Point2D>& pts){
    if(pts.size()<10) return {};
    std::vector<std::vector<Point2D>> segs;
    std::vector<Point2D> cur; cur.push_back(pts[0]);
    for(size_t i=1;i<pts.size();++i){
        if((pts[i]-pts[i-1]).norm()>0.3){if(cur.size()>=5)segs.push_back(cur);cur.clear();}
        cur.push_back(pts[i]);
    }
    if(cur.size()>=5) segs.push_back(cur);
    std::vector<LineSegment> all_lines;
    for(auto& s:segs) split_and_merge(s,0,(int)s.size(),0.05,all_lines);
    return extract_corners(all_lines);
}

IcpResult scan_icp(const std::vector<Point2D>& scan_ref,
                   const std::vector<Point2D>& scan_src,
                   double init_dx,double init_dy,double init_dth,
                   int max_iters=50,double conv_thresh=5e-5,double max_corr=0.5)
{
    IcpResult res{init_dx,init_dy,init_dth,1e9,0.0,0,false};
    if(scan_ref.size()<10||scan_src.size()<10) return res;
    std::vector<Point2D> ref_ds,src_ds;
    for(size_t i=0;i<scan_ref.size();i+=2) ref_ds.push_back(scan_ref[i]);
    for(size_t i=0;i<scan_src.size();i+=2) src_ds.push_back(scan_src[i]);
    if(ref_ds.size()<10||src_ds.size()<10) return res;
    double c0=std::cos(init_dth),s0=std::sin(init_dth);
    Eigen::Matrix2d R_total; R_total<<c0,-s0,s0,c0;
    Eigen::Vector2d t_total(init_dx,init_dy);
    for(int iter=0;iter<max_iters;++iter){
        std::vector<Point2D> src_t;
        for(auto& p:src_ds) src_t.push_back(R_total*p+t_total);
        std::vector<std::pair<Point2D,Point2D>> corr; double rms_sum=0;
        for(auto& sp:src_t){
            double best_d=1e9; size_t best_j=0;
            for(size_t j=0;j<ref_ds.size();++j){double d=(ref_ds[j]-sp).norm();if(d<best_d){best_d=d;best_j=j;}}
            if(best_d<max_corr){corr.emplace_back(ref_ds[best_j],sp);rms_sum+=best_d*best_d;}
        }
        res.corr_ratio=(double)corr.size()/(double)src_ds.size();
        if(corr.size()<10||res.corr_ratio<0.25) break;
        res.rms=std::sqrt(rms_sum/corr.size()); res.iterations=iter+1;
        Eigen::Vector2d mu_r{0,0},mu_s{0,0};
        for(auto& [r,sv]:corr){mu_r+=r;mu_s+=sv;}
        mu_r/=(double)corr.size(); mu_s/=(double)corr.size();
        Eigen::Matrix2d W=Eigen::Matrix2d::Zero();
        for(auto& [r,sv]:corr) W+=(r-mu_r)*(sv-mu_s).transpose();
        Eigen::JacobiSVD<Eigen::Matrix2d> svd(W,Eigen::ComputeFullU|Eigen::ComputeFullV);
        Eigen::Matrix2d R_inc=svd.matrixU()*svd.matrixV().transpose();
        if(R_inc.determinant()<0){Eigen::Matrix2d U=svd.matrixU();U.col(1)*=-1;R_inc=U*svd.matrixV().transpose();}
        Eigen::Vector2d t_inc=mu_r-R_inc*mu_s;
        t_total=R_inc*t_total+t_inc; R_total=R_inc*R_total;
        Eigen::JacobiSVD<Eigen::Matrix2d> svd2(R_total,Eigen::ComputeFullU|Eigen::ComputeFullV);
        R_total=svd2.matrixU()*svd2.matrixV().transpose();
        if(t_inc.norm()<conv_thresh&&std::abs(std::atan2(R_inc(1,0),R_inc(0,0)))<conv_thresh)
            {res.converged=true;break;}
    }
    {double rs=0;int nc=0;
     for(auto& p:src_ds){Point2D sp=R_total*p+t_total;double bd=1e9;
         for(auto& rp:ref_ds) bd=std::min(bd,(rp-sp).norm());
         if(bd<max_corr){rs+=bd*bd;nc++;}}
     if(nc>=10){res.rms=std::sqrt(rs/nc);res.corr_ratio=(double)nc/(double)src_ds.size();
         if(!res.converged&&res.rms<0.02) res.converged=true;}}
    res.dth=std::atan2(R_total(1,0),R_total(0,0));
    res.dx=t_total.x(); res.dy=t_total.y();
    return res;
}

std::vector<Point2D> scan_to_points(const sensor_msgs::msg::LaserScan& scan){
    std::vector<Point2D> pts; float angle=scan.angle_min;
    for(size_t i=0;i<scan.ranges.size();++i){
        float r=scan.ranges[i];
        if(r>scan.range_min&&r<scan.range_max&&std::isfinite(r))
            pts.push_back({r*std::cos(angle),r*std::sin(angle)});
        angle+=scan.angle_increment;
    }
    return pts;
}

class GraphBuilderNode : public rclcpp::Node {
public:
    GraphBuilderNode() : Node("graph_builder_node"), rng_(std::random_device{}())
    {

        min_dist_   = declare_parameter("min_node_dist",   0.2);

        odom_noise_enabled_ = declare_parameter("odom_noise_enabled", false);
        noise_alpha1_ = declare_parameter("noise_alpha1", 0.02);
        noise_alpha2_ = declare_parameter("noise_alpha2", 0.005);
        noise_alpha3_ = declare_parameter("noise_alpha3", 0.005);
        noise_alpha4_ = declare_parameter("noise_alpha4", 0.005);

        bool n = odom_noise_enabled_;

        lc_min_gap_         = declare_parameter("lc_min_node_gap",   n ? 10   : 20);
        lc_cooldown_        = declare_parameter("lc_cooldown",        n ? 10   : 30);
        lc_max_dth_         = declare_parameter("lc_max_dth",         n ? 6.3  : 0.2);
        icp_max_result_dth_ = declare_parameter("icp_max_result_dth", n ? 0.30 : 0.15);
        icp_min_ratio_      = declare_parameter("icp_min_ratio",      n ? 0.80 : 0.35);
        lc_max_corr_        = declare_parameter("lc_max_corr",        0.5);
        lc_rms_thresh_      = declare_parameter("lc_rms_thresh",      n ? 0.04 : 0.03);
        lc_max_iters_       = declare_parameter("lc_max_iters",       50);
        lc_disagree_max_    = declare_parameter("lc_disagree_max",    n ? 2.0  : 0.3);
        lc_max_z_trans_     = declare_parameter("lc_max_z_trans",     n ? 2.0  : 0.3);
        lc_dist_            = declare_parameter("lc_search_dist",     n ? 2.5  : 0.5);

        RCLCPP_INFO(get_logger(),
            "LC params: search_dist=%.1f cooldown=%d gap=%d "
            "icp_max_dth=%.2f icp_min_ratio=%.2f lc_max_z=%.1f",
            lc_dist_, lc_cooldown_, lc_min_gap_,
            icp_max_result_dth_, icp_min_ratio_, lc_max_z_trans_);

        RCLCPP_INFO(get_logger(),
            "GraphBuilder v11 (CLEAN STACHNISS). noise=%s lc_dist=%.2f",
            odom_noise_enabled_?"ON (α=[%.2f,%.2f,%.2f,%.2f])":"OFF", lc_dist_);
        if (odom_noise_enabled_)
            RCLCPP_WARN(get_logger(),
                "NOISE ON: α1=%.2f α2=%.2f α3=%.2f α4=%.2f — "
                "expect visible drift after 1-2 laps, LC will snap it",
                noise_alpha1_,noise_alpha2_,noise_alpha3_,noise_alpha4_);

        RCLCPP_INFO(get_logger(),"Consecutive edges: RAW ODOMETRY z (Stachniss slide 14)");
        RCLCPP_INFO(get_logger(),"LC edges: ICP z (Stachniss slide 16)");

        odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(
            "/odom",10,std::bind(&GraphBuilderNode::odom_cb,this,std::placeholders::_1));
        scan_sub_ = create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan",10,std::bind(&GraphBuilderNode::scan_cb,this,std::placeholders::_1));
        path_sub_ = create_subscription<nav_msgs::msg::Path>(
            "/optimized_path",10,
            std::bind(&GraphBuilderNode::path_cb,this,std::placeholders::_1));
        graph_pub_ = create_publisher<slam_msgs::msg::PoseGraph>("/pose_graph",10);
        timer_ = create_wall_timer(std::chrono::seconds(2),
                     std::bind(&GraphBuilderNode::publish_graph,this));
        nodes_.push_back({0.0,0.0,0.0, 0.0,0.0,0.0, 0,{},{}});
        RCLCPP_INFO(get_logger(),"Node 0 at origin.");
    }

private:

    void path_cb(const nav_msgs::msg::Path::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(graph_mutex_);
        size_t n=std::min(msg->poses.size(), nodes_.size());
        for(size_t i=0;i<n;++i){
            nodes_[i].x  = msg->poses[i].pose.position.x;
            nodes_[i].y  = msg->poses[i].pose.position.y;
            nodes_[i].th = 2.0*std::atan2(
                msg->poses[i].pose.orientation.z,
                msg->poses[i].pose.orientation.w);
        }
    }

    void scan_cb(const sensor_msgs::msg::LaserScan::SharedPtr msg){
        std::lock_guard<std::mutex> lock(scan_mutex_);
        latest_scan_=scan_to_points(*msg);
        if(!nodes_.empty()&&nodes_.back().scan.empty()){
            nodes_.back().scan=latest_scan_;
            nodes_.back().corners=extract_features(latest_scan_);
        }
    }

    void odom_cb(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        double x=msg->pose.pose.position.x,y=msg->pose.pose.position.y;
        tf2::Quaternion q(msg->pose.pose.orientation.x,msg->pose.pose.orientation.y,
                          msg->pose.pose.orientation.z,msg->pose.pose.orientation.w);
        double th=tf2::getYaw(q);
        if(!has_prev_odom_){prev_odom_x_=x;prev_odom_y_=y;prev_odom_th_=th;has_prev_odom_=true;return;}

        double dx_w=x-prev_odom_x_, dy_w=y-prev_odom_y_;
        double dth=norm_angle(th-prev_odom_th_);
        double dist=std::hypot(dx_w,dy_w);

        if(dist < min_dist_) return;

        double c_=std::cos(prev_odom_th_),s_=std::sin(prev_odom_th_);
        double z_x =  c_*dx_w + s_*dy_w;
        double z_y = -s_*dx_w + c_*dy_w;
        double z_th = dth;

        double vt = 0.1*dist*dist + 0.01*dth*dth + 1e-4;
        double vr = 0.01*dist*dist + 0.1*dth*dth  + 1e-4;
        double it = std::clamp(1.0/vt, 10.0, 100.0);
        double ir = std::clamp(1.0/vr,  5.0,  50.0);

        double noise_x=0, noise_y=0, noise_th=0;
        if(odom_noise_enabled_){
            double sigma_t = std::sqrt(noise_alpha1_*dist*dist + noise_alpha2_*dth*dth);
            double sigma_r = std::sqrt(noise_alpha3_*dist*dist + noise_alpha4_*dth*dth);
            std::normal_distribution<double> nd_t(0.0,sigma_t), nd_r(0.0,sigma_r);
            double n_lx=nd_t(rng_), n_ly=nd_t(rng_); noise_th=nd_r(rng_);
            std::lock_guard<std::mutex> lock(graph_mutex_);
            double cp2=std::cos(nodes_.back().raw_th),sp2=std::sin(nodes_.back().raw_th);
            noise_x=cp2*n_lx-sp2*n_ly; noise_y=sp2*n_lx+cp2*n_ly;
        }

        std::lock_guard<std::mutex> lock(graph_mutex_);
        int from_id=(int)nodes_.size()-1, to_id=(int)nodes_.size();

        slam_msgs::msg::PoseEdge edge;
        edge.from_id=from_id; edge.to_id=to_id;
        edge.z={z_x,z_y,z_th,0,0,0};
        edge.omega={it,0,0,0,it,0,0,0,ir};
        edge.edge_type="odometry";
        edges_.push_back({edge});

        const auto& prev=nodes_.back();
        double cp=std::cos(prev.raw_th),sp=std::sin(prev.raw_th);
        double nx  = prev.raw_x + cp*z_x - sp*z_y + noise_x;
        double ny  = prev.raw_y + sp*z_x + cp*z_y + noise_y;
        double nth = norm_angle(prev.raw_th + z_th + noise_th);

        std::vector<Point2D> cur_scan; std::vector<Corner> cur_corners;
        {std::lock_guard<std::mutex> lock2(scan_mutex_);
         cur_scan=latest_scan_; cur_corners=extract_features(latest_scan_);}

        nodes_.push_back({nx,ny,nth, nx,ny,nth, to_id,cur_scan,cur_corners});
        prev_odom_x_=x; prev_odom_y_=y; prev_odom_th_=th;
        check_loop_closure(to_id);
    }

    void check_loop_closure(int current_id)
    {
        if(current_id<lc_min_gap_) return;
        if(current_id-last_lc_node_<lc_cooldown_) return;
        const auto& cur=nodes_[current_id];
        if(cur.scan.empty()) return;

        for(int i=0;i<current_id-lc_min_gap_;++i){
            const auto& cand=nodes_[i];
            if(cand.scan.empty()) continue;

            double dist=std::hypot(cur.x-cand.x,cur.y-cand.y);
            if(dist>lc_dist_) continue;

            double dth_est=std::abs(norm_angle(cur.th-cand.th));
            if(dth_est>lc_max_dth_) continue;

            double ci=std::cos(cand.th),si=std::sin(cand.th);
            double ddx=cur.x-cand.x,ddy=cur.y-cand.y;
            double odom_px= ci*ddx+si*ddy;
            double odom_py=-si*ddx+ci*ddy;
            double odom_pth=norm_angle(cur.th-cand.th);

            double init_dx=odom_px,init_dy=odom_py,init_dth=odom_pth;
            bool used_feat=false; int n_feat=0;
            if(cand.corners.size()>=3&&cur.corners.size()>=3){
                struct Match{int a,b;double dd;};
                std::vector<Match> cands2;
                for(int a=0;a<(int)cand.corners.size();++a)
                    for(int b=0;b<(int)cur.corners.size();++b){
                        double dd=std::abs(cand.corners[a].descriptor-cur.corners[b].descriptor);
                        if(dd<0.25) cands2.push_back({a,b,dd});}
                std::sort(cands2.begin(),cands2.end(),[](const Match& x,const Match& y){return x.dd<y.dd;});
                std::vector<bool> ua(cand.corners.size(),false),ub(cur.corners.size(),false);
                std::vector<Match> matched;
                for(auto& m:cands2){if(ua[m.a]||ub[m.b])continue;matched.push_back(m);ua[m.a]=true;ub[m.b]=true;}
                n_feat=(int)matched.size();
                if(n_feat>=3){
                    Eigen::MatrixXd Pi(2,n_feat),Pj(2,n_feat);
                    for(int k=0;k<n_feat;++k){Pi.col(k)=cand.corners[matched[k].a].pos;Pj.col(k)=cur.corners[matched[k].b].pos;}
                    Eigen::Vector2d mi=Pi.rowwise().mean(),mj=Pj.rowwise().mean();
                    Eigen::Matrix2d Ws=(Pi.colwise()-mi)*(Pj.colwise()-mj).transpose();
                    Eigen::JacobiSVD<Eigen::Matrix2d> sv(Ws,Eigen::ComputeFullU|Eigen::ComputeFullV);
                    Eigen::Matrix2d Rs=sv.matrixV()*sv.matrixU().transpose();
                    if(Rs.determinant()<0){Eigen::Matrix2d V=sv.matrixV();V.col(1)*=-1;Rs=V*sv.matrixU().transpose();}
                    Eigen::Vector2d ts=mj-Rs*mi;
                    double svd_err=0;for(int k=0;k<n_feat;++k)svd_err+=(Rs*Pi.col(k)+ts-Pj.col(k)).squaredNorm();
                    svd_err/=n_feat;
                    if(svd_err<0.08){init_dx=ts.x();init_dy=ts.y();init_dth=std::atan2(Rs(1,0),Rs(0,0));used_feat=true;}
                }
            }

            IcpResult icp=scan_icp(cand.scan,cur.scan,init_dx,init_dy,init_dth,
                                   lc_max_iters_,5e-5,lc_max_corr_);
            if(!icp.converged) continue;
            if(icp.rms>lc_rms_thresh_){
                RCLCPP_DEBUG(get_logger(),"LC %d<->%d: rms=%.4f rejected",i,current_id,icp.rms);
                continue;}
            if(icp.corr_ratio<icp_min_ratio_) continue;

            double z_trans=std::hypot(icp.dx,icp.dy);
            if(z_trans>lc_max_z_trans_){
                RCLCPP_WARN(get_logger(),"LC %d<->%d: |z_trans|=%.3f>%.2f REJECTED z=[%.3f,%.3f,%.3f]",
                    i,current_id,z_trans,lc_max_z_trans_,icp.dx,icp.dy,icp.dth);
                continue;}

            double abs_dth=std::abs(norm_angle(icp.dth));
            if(abs_dth>icp_max_result_dth_){
                RCLCPP_WARN(get_logger(),"LC %d<->%d: |z.dth|=%.3f REJECTED",i,current_id,abs_dth);
                continue;}

            double t_dis=std::hypot(icp.dx-odom_px,icp.dy-odom_py);
            if(t_dis>lc_disagree_max_){
                RCLCPP_WARN(get_logger(),"LC %d<->%d: t_dis=%.3f REJECTED",i,current_id,t_dis);
                continue;}

            if(!odom_noise_enabled_){
                double r_dis=std::abs(norm_angle(icp.dth-odom_pth));
                if(r_dis>icp_max_result_dth_){
                    RCLCPP_WARN(get_logger(),"LC %d<->%d: r_dis=%.3f REJECTED",i,current_id,r_dis);
                    continue;}
            }

            double q_=1.0/(icp.rms+0.01);
            double lc_it, lc_ir;
            if(odom_noise_enabled_){
                lc_it=std::clamp(q_*20.0, 200.0, 1000.0);
                lc_ir=std::clamp(q_*10.0, 100.0,  500.0);
            } else {
                lc_it=std::clamp(q_*10.0, 100.0,  500.0);
                lc_ir=std::clamp(q_* 5.0,  50.0,  250.0);
            }

            slam_msgs::msg::PoseEdge lc;
            lc.from_id=i; lc.to_id=current_id; lc.edge_type="loop_closure";
            lc.z={icp.dx,icp.dy,icp.dth,0,0,0};
            lc.omega={lc_it,0,0,0,lc_it,0,0,0,lc_ir};
            edges_.push_back({lc});
            last_lc_node_=current_id;

            RCLCPP_INFO(get_logger(),
                "★ LC %d<->%d dist=%.3f feat=%s n=%d "
                "rms=%.4f ratio=%.2f z=[%.3f,%.3f,%.3f] "
                "z_trans=%.3f |z.dth|=%.3f t_dis=%.3f Ω_lc=%.0f",
                i,current_id,dist,used_feat?"SVD":"odom",n_feat,
                icp.rms,icp.corr_ratio,
                icp.dx,icp.dy,icp.dth,z_trans,abs_dth,t_dis,lc_it);
            break;
        }
    }

    void publish_graph(){
        std::lock_guard<std::mutex> lock(graph_mutex_);
        if(nodes_.size()<2) return;
        slam_msgs::msg::PoseGraph g;
        g.header.stamp=now(); g.header.frame_id="map";
        g.num_nodes=(int)nodes_.size();

        for(auto& n:nodes_){
            g.node_poses.push_back(n.raw_x);
            g.node_poses.push_back(n.raw_y);
            g.node_poses.push_back(n.raw_th);
        }
        for(auto& e:edges_) g.edges.push_back(e.msg);
        graph_pub_->publish(g);
        int n_lc=0,tc=0;
        for(auto& e:edges_) if(e.msg.edge_type=="loop_closure") n_lc++;
        for(auto& n:nodes_) tc+=(int)n.corners.size();
        RCLCPP_INFO_THROTTLE(get_logger(),*get_clock(),10000,
            "Graph: %d nodes %zu edges (%d LC) %d corners",
            g.num_nodes,edges_.size(),n_lc,tc);
    }

    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr     odom_sub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr          path_sub_;
    rclcpp::Publisher<slam_msgs::msg::PoseGraph>::SharedPtr       graph_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::mutex graph_mutex_, scan_mutex_;
    std::vector<PoseNode> nodes_; std::vector<Edge> edges_;
    std::vector<Point2D> latest_scan_;
    double prev_odom_x_{},prev_odom_y_{},prev_odom_th_{};
    bool has_prev_odom_=false;
    std::mt19937 rng_;
    double min_dist_,lc_dist_,lc_max_dth_;
    int lc_min_gap_,lc_cooldown_,last_lc_node_=-100;
    bool odom_noise_enabled_;
    double noise_alpha1_,noise_alpha2_,noise_alpha3_,noise_alpha4_;
    double lc_max_corr_,lc_rms_thresh_,lc_disagree_max_,lc_max_z_trans_;
    int lc_max_iters_;
    double icp_min_ratio_,icp_max_result_dth_;
};

int main(int argc,char** argv){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<GraphBuilderNode>());
    rclcpp::shutdown();
}