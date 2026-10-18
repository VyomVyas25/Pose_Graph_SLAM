// ============================================================
//  PointCloudMapNode - map from the optimised graph
//
//  Each graph node's scan (/node_scans: base frame at the node's time, lidar
//  offset applied, deskewed - from the front end) is drawn at that node's
//  OPTIMISED pose (/optimized_path, index = node id). Every new optimisation
//  redraws the whole map, so loop closures straighten old walls too.
// ============================================================
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "nav_msgs/msg/path.hpp"
#include "slam_msgs/msg/node_scan.hpp"
#include <tf2/utils.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <cmath>
#include <cstring>
#include <map>
#include <mutex>
#include <vector>

class PointCloudMapNode : public rclcpp::Node {
public:
    PointCloudMapNode() : Node("point_cloud_map_node")
    {
        scan_skip_ = declare_parameter("scan_skip", 3);           // keep every n-th point of a node scan
        max_range_ = declare_parameter("max_range", 3.4);         // m from the robot - far points are the least accurate
        map_frame_ = declare_parameter("map_frame", std::string("map"));

        scan_sub_ = create_subscription<slam_msgs::msg::NodeScan>("/node_scans", rclcpp::QoS(2000).reliable(),
            std::bind(&PointCloudMapNode::scan_cb, this, std::placeholders::_1));
        path_sub_ = create_subscription<nav_msgs::msg::Path>("/optimized_path", 10,
            std::bind(&PointCloudMapNode::path_cb, this, std::placeholders::_1));
        cloud_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/point_cloud_map", 10);
        timer_ = create_wall_timer(std::chrono::seconds(1), std::bind(&PointCloudMapNode::publish_cloud, this));
        RCLCPP_INFO(get_logger(), "PointCloudMapNode ready. scan_skip=%d max_range=%.1f", scan_skip_, max_range_);
    }

private:
    struct Pose { double x, y, th; };

    void scan_cb(const slam_msgs::msg::NodeScan::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& pts = scans_[msg->id];
        for (size_t i = 0; i < msg->x.size(); i += scan_skip_)
            if (std::hypot(msg->x[i], msg->y[i]) < max_range_) pts.push_back({msg->x[i], msg->y[i]});
        needs_rebuild_ = true;
    }

    void path_cb(const nav_msgs::msg::Path::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        poses_.clear();
        for (auto& ps : msg->poses) poses_.push_back({ps.pose.position.x, ps.pose.position.y, tf2::getYaw(ps.pose.orientation)});
        needs_rebuild_ = true;
    }

    void publish_cloud()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!needs_rebuild_ || poses_.empty()) return;
        std::vector<float> xy;
        for (const auto& [id, pts] : scans_) {
            if (id < 0 || id >= static_cast<int>(poses_.size())) continue;   // not optimised yet
            const Pose& p = poses_[id]; const double c = std::cos(p.th), s = std::sin(p.th);
            for (const auto& [lx, ly] : pts) { xy.push_back(static_cast<float>(p.x + c * lx - s * ly)); xy.push_back(static_cast<float>(p.y + s * lx + c * ly)); }
        }
        const size_t total = xy.size() / 2;
        if (total == 0) return;

        sensor_msgs::msg::PointCloud2 cloud;
        cloud.header.frame_id = map_frame_;
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
        uint8_t* ptr = cloud.data.data(); const float z = 0.0f;
        for (size_t i = 0; i < total; ++i) {
            std::memcpy(ptr + 0, &xy[2 * i], 4); std::memcpy(ptr + 4, &xy[2 * i + 1], 4); std::memcpy(ptr + 8, &z, 4); ptr += 12;
        }
        cloud_pub_->publish(cloud);
        needs_rebuild_ = false;
        RCLCPP_INFO_THROTTLE(get_logger(),*get_clock(),10000, "PointCloud: %zu node scans  %zu points", scans_.size(), total);
    }

    rclcpp::Subscription<slam_msgs::msg::NodeScan>::SharedPtr     scan_sub_;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr          path_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr   cloud_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::mutex mutex_;
    std::map<int, std::vector<std::pair<float, float>>> scans_;
    std::vector<Pose> poses_;
    bool needs_rebuild_ = false;
    int scan_skip_;
    double max_range_;
    std::string map_frame_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<PointCloudMapNode>());
    rclcpp::shutdown();
    return 0;
}
