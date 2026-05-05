// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from slam_msgs:msg/PoseGraph.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__STRUCT_HPP_
#define SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"
// Member 'edges'
#include "slam_msgs/msg/detail/pose_edge__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__slam_msgs__msg__PoseGraph __attribute__((deprecated))
#else
# define DEPRECATED__slam_msgs__msg__PoseGraph __declspec(deprecated)
#endif

namespace slam_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct PoseGraph_
{
  using Type = PoseGraph_<ContainerAllocator>;

  explicit PoseGraph_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->num_nodes = 0l;
    }
  }

  explicit PoseGraph_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->num_nodes = 0l;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _node_poses_type =
    std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>>;
  _node_poses_type node_poses;
  using _num_nodes_type =
    int32_t;
  _num_nodes_type num_nodes;
  using _edges_type =
    std::vector<slam_msgs::msg::PoseEdge_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<slam_msgs::msg::PoseEdge_<ContainerAllocator>>>;
  _edges_type edges;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__node_poses(
    const std::vector<double, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<double>> & _arg)
  {
    this->node_poses = _arg;
    return *this;
  }
  Type & set__num_nodes(
    const int32_t & _arg)
  {
    this->num_nodes = _arg;
    return *this;
  }
  Type & set__edges(
    const std::vector<slam_msgs::msg::PoseEdge_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<slam_msgs::msg::PoseEdge_<ContainerAllocator>>> & _arg)
  {
    this->edges = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    slam_msgs::msg::PoseGraph_<ContainerAllocator> *;
  using ConstRawPtr =
    const slam_msgs::msg::PoseGraph_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      slam_msgs::msg::PoseGraph_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      slam_msgs::msg::PoseGraph_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__slam_msgs__msg__PoseGraph
    std::shared_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__slam_msgs__msg__PoseGraph
    std::shared_ptr<slam_msgs::msg::PoseGraph_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const PoseGraph_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->node_poses != other.node_poses) {
      return false;
    }
    if (this->num_nodes != other.num_nodes) {
      return false;
    }
    if (this->edges != other.edges) {
      return false;
    }
    return true;
  }
  bool operator!=(const PoseGraph_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct PoseGraph_

// alias to use template instance with default allocator
using PoseGraph =
  slam_msgs::msg::PoseGraph_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace slam_msgs

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__STRUCT_HPP_
