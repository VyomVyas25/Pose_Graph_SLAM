#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "nav_msgs/msg/path.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>
#include <vector>
#include <cmath>
#include <mutex>
#include <cstring>
#include <limits>

struct ScanSnapshot {
    double orig_x, orig_y;
    double x, y, th;
    std::vector<std::pair<float,float>> pts;
};

class PointCloudMapNode : public rclcpp::Node {
public:
    PointCloudMapNode() : Node("point_cloud_map_node")
    {
        scan_skip_ = declare_parameter("scan_skip",    3);
        min_range_ = declare_parameter("min_range",    0.1);
        max_range_ = declare_parameter("max_range",    3.4);

        snap_dist_ = declare_parameter("snap_dist",    0.20);

        scan_sub_ = create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10,
            std::bind(&PointCloudMapNode::scan_cb, this, std::placeholders::_1));
        odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 10,
            std::bind(&PointCloudMapNode::odom_cb, this, std::placeholders::_1));
        path_sub_ = create_subscription<nav_msgs::msg::Path>(
            "/optimized_path", 10,
            std::bind(&PointCloudMapNode::path_cb, this, std::placeholders::_1));
        cloud_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>(
            "/point_cloud_map", rclcpp::QoS(1).transient_local());
        timer_ = create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&PointCloudMapNode::publish_cloud, this));

        RCLCPP_INFO(get_logger(),
            "PointCloudMapNode ready. scan_skip=%d max_range=%.1f snap_dist=%.2f",
            scan_skip_, max_range_, snap_dist_);
    }

private:
    void odom_cb(const nav_msgs::msg::Odometry::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_x_ = msg->pose.pose.position.x;
        robot_y_ = msg->pose.pose.position.y;
        tf2::Quaternion q(
            msg->pose.pose.orientation.x, msg->pose.pose.orientation.y,
            msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
        robot_th_ = tf2::getYaw(q);
        has_pose_ = true;
    }

    void scan_cb(const sensor_msgs::msg::LaserScan::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!has_pose_) return;

        double dx = robot_x_ - last_snap_x_;
        double dy = robot_y_ - last_snap_y_;
        if (std::hypot(dx, dy) < snap_dist_ && !snapshots_.empty()) return;

        ScanSnapshot snap;
        snap.orig_x = robot_x_;
        snap.orig_y = robot_y_;
        snap.x      = robot_x_;
        snap.y      = robot_y_;
        snap.th     = robot_th_;

        float angle = msg->angle_min;
        for (size_t i = 0; i < msg->ranges.size(); ++i) {
            float r = msg->ranges[i];
            if (i % scan_skip_ == 0 &&
                r > min_range_ && r < max_range_ && std::isfinite(r)) {
                snap.pts.push_back({r*std::cos(angle), r*std::sin(angle)});
            }
            angle += msg->angle_increment;
        }

        snapshots_.push_back(snap);
        last_snap_x_ = robot_x_;
        last_snap_y_ = robot_y_;
        needs_rebuild_ = true;
    }

    void path_cb(const nav_msgs::msg::Path::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (msg->poses.empty() || snapshots_.empty()) return;

        struct PP { double x, y, th; };
        std::vector<PP> pp;
        pp.reserve(msg->poses.size());
        for (auto& ps : msg->poses) {
            double th = 2.0*std::atan2(
                ps.pose.orientation.z, ps.pose.orientation.w);
            pp.push_back({ps.pose.position.x, ps.pose.position.y, th});
        }

        for (auto& snap : snapshots_) {
            double best_d = std::numeric_limits<double>::max();
            size_t best_i = 0;
            for (size_t i = 0; i < pp.size(); ++i) {
                double d = std::hypot(pp[i].x - snap.orig_x,
                                      pp[i].y - snap.orig_y);
                if (d < best_d) { best_d = d; best_i = i; }
            }
            snap.x  = pp[best_i].x;
            snap.y  = pp[best_i].y;
            snap.th = pp[best_i].th;
        }

        needs_rebuild_ = true;
    }

    void publish_cloud()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (snapshots_.empty() || !needs_rebuild_) return;

        size_t total = 0;
        for (auto& s : snapshots_) total += s.pts.size();
        if (total == 0) return;

        sensor_msgs::msg::PointCloud2 cloud;
        cloud.header.frame_id = "map";
        cloud.header.stamp    = now();
        cloud.height = 1; cloud.width = total;
        cloud.is_bigendian = false; cloud.is_dense = false;
        cloud.fields.resize(3);
        for (int k=0;k<3;++k){
            cloud.fields[k].datatype=sensor_msgs::msg::PointField::FLOAT32;
            cloud.fields[k].count=1;
        }
        cloud.fields[0].name="x"; cloud.fields[0].offset=0;
        cloud.fields[1].name="y"; cloud.fields[1].offset=4;
        cloud.fields[2].name="z"; cloud.fields[2].offset=8;
        cloud.point_step=12;
        cloud.row_step=cloud.point_step*total;
        cloud.data.resize(cloud.row_step);

        uint8_t* ptr = cloud.data.data();
        for (auto& snap : snapshots_) {
            double c=std::cos(snap.th), s=std::sin(snap.th);
            for (auto& [lx,ly] : snap.pts) {
                float gx=(float)(snap.x+c*lx-s*ly);
                float gy=(float)(snap.y+s*lx+c*ly);
                float gz=0.0f;
                std::memcpy(ptr+0,&gx,4);
                std::memcpy(ptr+4,&gy,4);
                std::memcpy(ptr+8,&gz,4);
                ptr+=12;
            }
        }

        cloud_pub_->publish(cloud);
        needs_rebuild_ = false;

        RCLCPP_INFO_THROTTLE(get_logger(),*get_clock(),10000,
            "PointCloud: %zu snapshots  %zu points",
            snapshots_.size(), total);
    }

    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr  scan_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr       odom_sub_;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr           path_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr    cloud_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::mutex mutex_;
    std::vector<ScanSnapshot> snapshots_;
    double robot_x_{},robot_y_{},robot_th_{};
    bool   has_pose_=false, needs_rebuild_=false;
    double last_snap_x_=1e9, last_snap_y_=1e9;
    int    scan_skip_;
    double min_range_,max_range_,snap_dist_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<PointCloudMapNode>());
    rclcpp::shutdown();
}