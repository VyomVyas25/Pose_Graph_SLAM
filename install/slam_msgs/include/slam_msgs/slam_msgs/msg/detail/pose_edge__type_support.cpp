// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "slam_msgs/msg/detail/pose_edge__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace slam_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void PoseEdge_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) slam_msgs::msg::PoseEdge(_init);
}

void PoseEdge_fini_function(void * message_memory)
{
  auto typed_message = static_cast<slam_msgs::msg::PoseEdge *>(message_memory);
  typed_message->~PoseEdge();
}

size_t size_function__PoseEdge__z(const void * untyped_member)
{
  (void)untyped_member;
  return 6;
}

const void * get_const_function__PoseEdge__z(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::array<double, 6> *>(untyped_member);
  return &member[index];
}

void * get_function__PoseEdge__z(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::array<double, 6> *>(untyped_member);
  return &member[index];
}

void fetch_function__PoseEdge__z(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const double *>(
    get_const_function__PoseEdge__z(untyped_member, index));
  auto & value = *reinterpret_cast<double *>(untyped_value);
  value = item;
}

void assign_function__PoseEdge__z(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<double *>(
    get_function__PoseEdge__z(untyped_member, index));
  const auto & value = *reinterpret_cast<const double *>(untyped_value);
  item = value;
}

size_t size_function__PoseEdge__omega(const void * untyped_member)
{
  (void)untyped_member;
  return 9;
}

const void * get_const_function__PoseEdge__omega(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::array<double, 9> *>(untyped_member);
  return &member[index];
}

void * get_function__PoseEdge__omega(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::array<double, 9> *>(untyped_member);
  return &member[index];
}

void fetch_function__PoseEdge__omega(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const double *>(
    get_const_function__PoseEdge__omega(untyped_member, index));
  auto & value = *reinterpret_cast<double *>(untyped_value);
  value = item;
}

void assign_function__PoseEdge__omega(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<double *>(
    get_function__PoseEdge__omega(untyped_member, index));
  const auto & value = *reinterpret_cast<const double *>(untyped_value);
  item = value;
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember PoseEdge_message_member_array[5] = {
  {
    "from_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs::msg::PoseEdge, from_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "to_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs::msg::PoseEdge, to_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "z",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    true,  // is array
    6,  // array size
    false,  // is upper bound
    offsetof(slam_msgs::msg::PoseEdge, z),  // bytes offset in struct
    nullptr,  // default value
    size_function__PoseEdge__z,  // size() function pointer
    get_const_function__PoseEdge__z,  // get_const(index) function pointer
    get_function__PoseEdge__z,  // get(index) function pointer
    fetch_function__PoseEdge__z,  // fetch(index, &value) function pointer
    assign_function__PoseEdge__z,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "omega",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    true,  // is array
    9,  // array size
    false,  // is upper bound
    offsetof(slam_msgs::msg::PoseEdge, omega),  // bytes offset in struct
    nullptr,  // default value
    size_function__PoseEdge__omega,  // size() function pointer
    get_const_function__PoseEdge__omega,  // get_const(index) function pointer
    get_function__PoseEdge__omega,  // get(index) function pointer
    fetch_function__PoseEdge__omega,  // fetch(index, &value) function pointer
    assign_function__PoseEdge__omega,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "edge_type",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs::msg::PoseEdge, edge_type),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers PoseEdge_message_members = {
  "slam_msgs::msg",  // message namespace
  "PoseEdge",  // message name
  5,  // number of fields
  sizeof(slam_msgs::msg::PoseEdge),
  PoseEdge_message_member_array,  // message members
  PoseEdge_init_function,  // function to initialize message memory (memory has to be allocated)
  PoseEdge_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t PoseEdge_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &PoseEdge_message_members,
  get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace slam_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<slam_msgs::msg::PoseEdge>()
{
  return &::slam_msgs::msg::rosidl_typesupport_introspection_cpp::PoseEdge_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, slam_msgs, msg, PoseEdge)() {
  return &::slam_msgs::msg::rosidl_typesupport_introspection_cpp::PoseEdge_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
