// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_EDGE__STRUCT_HPP_
#define SLAM_MSGS__MSG__DETAIL__POSE_EDGE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__slam_msgs__msg__PoseEdge __attribute__((deprecated))
#else
# define DEPRECATED__slam_msgs__msg__PoseEdge __declspec(deprecated)
#endif

namespace slam_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct PoseEdge_
{
  using Type = PoseEdge_<ContainerAllocator>;

  explicit PoseEdge_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->from_id = 0l;
      this->to_id = 0l;
      std::fill<typename std::array<double, 6>::iterator, double>(this->z.begin(), this->z.end(), 0.0);
      std::fill<typename std::array<double, 9>::iterator, double>(this->omega.begin(), this->omega.end(), 0.0);
      this->edge_type = "";
    }
  }

  explicit PoseEdge_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : z(_alloc),
    omega(_alloc),
    edge_type(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->from_id = 0l;
      this->to_id = 0l;
      std::fill<typename std::array<double, 6>::iterator, double>(this->z.begin(), this->z.end(), 0.0);
      std::fill<typename std::array<double, 9>::iterator, double>(this->omega.begin(), this->omega.end(), 0.0);
      this->edge_type = "";
    }
  }

  // field types and members
  using _from_id_type =
    int32_t;
  _from_id_type from_id;
  using _to_id_type =
    int32_t;
  _to_id_type to_id;
  using _z_type =
    std::array<double, 6>;
  _z_type z;
  using _omega_type =
    std::array<double, 9>;
  _omega_type omega;
  using _edge_type_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _edge_type_type edge_type;

  // setters for named parameter idiom
  Type & set__from_id(
    const int32_t & _arg)
  {
    this->from_id = _arg;
    return *this;
  }
  Type & set__to_id(
    const int32_t & _arg)
  {
    this->to_id = _arg;
    return *this;
  }
  Type & set__z(
    const std::array<double, 6> & _arg)
  {
    this->z = _arg;
    return *this;
  }
  Type & set__omega(
    const std::array<double, 9> & _arg)
  {
    this->omega = _arg;
    return *this;
  }
  Type & set__edge_type(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->edge_type = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    slam_msgs::msg::PoseEdge_<ContainerAllocator> *;
  using ConstRawPtr =
    const slam_msgs::msg::PoseEdge_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      slam_msgs::msg::PoseEdge_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      slam_msgs::msg::PoseEdge_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__slam_msgs__msg__PoseEdge
    std::shared_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__slam_msgs__msg__PoseEdge
    std::shared_ptr<slam_msgs::msg::PoseEdge_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const PoseEdge_ & other) const
  {
    if (this->from_id != other.from_id) {
      return false;
    }
    if (this->to_id != other.to_id) {
      return false;
    }
    if (this->z != other.z) {
      return false;
    }
    if (this->omega != other.omega) {
      return false;
    }
    if (this->edge_type != other.edge_type) {
      return false;
    }
    return true;
  }
  bool operator!=(const PoseEdge_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct PoseEdge_

// alias to use template instance with default allocator
using PoseEdge =
  slam_msgs::msg::PoseEdge_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace slam_msgs

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_EDGE__STRUCT_HPP_
