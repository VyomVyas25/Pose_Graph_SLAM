// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from slam_msgs:msg/PoseGraph.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__BUILDER_HPP_
#define SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "slam_msgs/msg/detail/pose_graph__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace slam_msgs
{

namespace msg
{

namespace builder
{

class Init_PoseGraph_edges
{
public:
  explicit Init_PoseGraph_edges(::slam_msgs::msg::PoseGraph & msg)
  : msg_(msg)
  {}
  ::slam_msgs::msg::PoseGraph edges(::slam_msgs::msg::PoseGraph::_edges_type arg)
  {
    msg_.edges = std::move(arg);
    return std::move(msg_);
  }

private:
  ::slam_msgs::msg::PoseGraph msg_;
};

class Init_PoseGraph_num_nodes
{
public:
  explicit Init_PoseGraph_num_nodes(::slam_msgs::msg::PoseGraph & msg)
  : msg_(msg)
  {}
  Init_PoseGraph_edges num_nodes(::slam_msgs::msg::PoseGraph::_num_nodes_type arg)
  {
    msg_.num_nodes = std::move(arg);
    return Init_PoseGraph_edges(msg_);
  }

private:
  ::slam_msgs::msg::PoseGraph msg_;
};

class Init_PoseGraph_node_poses
{
public:
  explicit Init_PoseGraph_node_poses(::slam_msgs::msg::PoseGraph & msg)
  : msg_(msg)
  {}
  Init_PoseGraph_num_nodes node_poses(::slam_msgs::msg::PoseGraph::_node_poses_type arg)
  {
    msg_.node_poses = std::move(arg);
    return Init_PoseGraph_num_nodes(msg_);
  }

private:
  ::slam_msgs::msg::PoseGraph msg_;
};

class Init_PoseGraph_header
{
public:
  Init_PoseGraph_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PoseGraph_node_poses header(::slam_msgs::msg::PoseGraph::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_PoseGraph_node_poses(msg_);
  }

private:
  ::slam_msgs::msg::PoseGraph msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::slam_msgs::msg::PoseGraph>()
{
  return slam_msgs::msg::builder::Init_PoseGraph_header();
}

}  // namespace slam_msgs

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__BUILDER_HPP_
