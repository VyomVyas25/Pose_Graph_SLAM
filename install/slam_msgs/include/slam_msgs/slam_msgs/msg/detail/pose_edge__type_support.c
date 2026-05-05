// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "slam_msgs/msg/detail/pose_edge__rosidl_typesupport_introspection_c.h"
#include "slam_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "slam_msgs/msg/detail/pose_edge__functions.h"
#include "slam_msgs/msg/detail/pose_edge__struct.h"


// Include directives for member types
// Member `edge_type`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  slam_msgs__msg__PoseEdge__init(message_memory);
}

void slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_fini_function(void * message_memory)
{
  slam_msgs__msg__PoseEdge__fini(message_memory);
}

size_t slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__size_function__PoseEdge__z(
  const void * untyped_member)
{
  (void)untyped_member;
  return 6;
}

const void * slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_const_function__PoseEdge__z(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_function__PoseEdge__z(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__fetch_function__PoseEdge__z(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_const_function__PoseEdge__z(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__assign_function__PoseEdge__z(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_function__PoseEdge__z(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

size_t slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__size_function__PoseEdge__omega(
  const void * untyped_member)
{
  (void)untyped_member;
  return 9;
}

const void * slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_const_function__PoseEdge__omega(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_function__PoseEdge__omega(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__fetch_function__PoseEdge__omega(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_const_function__PoseEdge__omega(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__assign_function__PoseEdge__omega(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_function__PoseEdge__omega(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_member_array[5] = {
  {
    "from_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseEdge, from_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "to_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseEdge, to_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "z",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    6,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseEdge, z),  // bytes offset in struct
    NULL,  // default value
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__size_function__PoseEdge__z,  // size() function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_const_function__PoseEdge__z,  // get_const(index) function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_function__PoseEdge__z,  // get(index) function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__fetch_function__PoseEdge__z,  // fetch(index, &value) function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__assign_function__PoseEdge__z,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "omega",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    9,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseEdge, omega),  // bytes offset in struct
    NULL,  // default value
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__size_function__PoseEdge__omega,  // size() function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_const_function__PoseEdge__omega,  // get_const(index) function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__get_function__PoseEdge__omega,  // get(index) function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__fetch_function__PoseEdge__omega,  // fetch(index, &value) function pointer
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__assign_function__PoseEdge__omega,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "edge_type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseEdge, edge_type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_members = {
  "slam_msgs__msg",  // message namespace
  "PoseEdge",  // message name
  5,  // number of fields
  sizeof(slam_msgs__msg__PoseEdge),
  slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_member_array,  // message members
  slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_init_function,  // function to initialize message memory (memory has to be allocated)
  slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_type_support_handle = {
  0,
  &slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_slam_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, slam_msgs, msg, PoseEdge)() {
  if (!slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_type_support_handle.typesupport_identifier) {
    slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &slam_msgs__msg__PoseEdge__rosidl_typesupport_introspection_c__PoseEdge_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
