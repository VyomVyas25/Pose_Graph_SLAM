// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_EDGE__BUILDER_HPP_
#define SLAM_MSGS__MSG__DETAIL__POSE_EDGE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "slam_msgs/msg/detail/pose_edge__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace slam_msgs
{

namespace msg
{

namespace builder
{

class Init_PoseEdge_edge_type
{
public:
  explicit Init_PoseEdge_edge_type(::slam_msgs::msg::PoseEdge & msg)
  : msg_(msg)
  {}
  ::slam_msgs::msg::PoseEdge edge_type(::slam_msgs::msg::PoseEdge::_edge_type_type arg)
  {
    msg_.edge_type = std::move(arg);
    return std::move(msg_);
  }

private:
  ::slam_msgs::msg::PoseEdge msg_;
};

class Init_PoseEdge_omega
{
public:
  explicit Init_PoseEdge_omega(::slam_msgs::msg::PoseEdge & msg)
  : msg_(msg)
  {}
  Init_PoseEdge_edge_type omega(::slam_msgs::msg::PoseEdge::_omega_type arg)
  {
    msg_.omega = std::move(arg);
    return Init_PoseEdge_edge_type(msg_);
  }

private:
  ::slam_msgs::msg::PoseEdge msg_;
};

class Init_PoseEdge_z
{
public:
  explicit Init_PoseEdge_z(::slam_msgs::msg::PoseEdge & msg)
  : msg_(msg)
  {}
  Init_PoseEdge_omega z(::slam_msgs::msg::PoseEdge::_z_type arg)
  {
    msg_.z = std::move(arg);
    return Init_PoseEdge_omega(msg_);
  }

private:
  ::slam_msgs::msg::PoseEdge msg_;
};

class Init_PoseEdge_to_id
{
public:
  explicit Init_PoseEdge_to_id(::slam_msgs::msg::PoseEdge & msg)
  : msg_(msg)
  {}
  Init_PoseEdge_z to_id(::slam_msgs::msg::PoseEdge::_to_id_type arg)
  {
    msg_.to_id = std::move(arg);
    return Init_PoseEdge_z(msg_);
  }

private:
  ::slam_msgs::msg::PoseEdge msg_;
};

class Init_PoseEdge_from_id
{
public:
  Init_PoseEdge_from_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PoseEdge_to_id from_id(::slam_msgs::msg::PoseEdge::_from_id_type arg)
  {
    msg_.from_id = std::move(arg);
    return Init_PoseEdge_to_id(msg_);
  }

private:
  ::slam_msgs::msg::PoseEdge msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::slam_msgs::msg::PoseEdge>()
{
  return slam_msgs::msg::builder::Init_PoseEdge_from_id();
}

}  // namespace slam_msgs

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_EDGE__BUILDER_HPP_
