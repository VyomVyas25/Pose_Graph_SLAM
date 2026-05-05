// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from slam_msgs:msg/PoseGraph.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "slam_msgs/msg/detail/pose_graph__rosidl_typesupport_introspection_c.h"
#include "slam_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "slam_msgs/msg/detail/pose_graph__functions.h"
#include "slam_msgs/msg/detail/pose_graph__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `node_poses`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `edges`
#include "slam_msgs/msg/pose_edge.h"
// Member `edges`
#include "slam_msgs/msg/detail/pose_edge__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  slam_msgs__msg__PoseGraph__init(message_memory);
}

void slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_fini_function(void * message_memory)
{
  slam_msgs__msg__PoseGraph__fini(message_memory);
}

size_t slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__size_function__PoseGraph__node_poses(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_const_function__PoseGraph__node_poses(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_function__PoseGraph__node_poses(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__fetch_function__PoseGraph__node_poses(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_const_function__PoseGraph__node_poses(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__assign_function__PoseGraph__node_poses(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_function__PoseGraph__node_poses(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__resize_function__PoseGraph__node_poses(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

size_t slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__size_function__PoseGraph__edges(
  const void * untyped_member)
{
  const slam_msgs__msg__PoseEdge__Sequence * member =
    (const slam_msgs__msg__PoseEdge__Sequence *)(untyped_member);
  return member->size;
}

const void * slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_const_function__PoseGraph__edges(
  const void * untyped_member, size_t index)
{
  const slam_msgs__msg__PoseEdge__Sequence * member =
    (const slam_msgs__msg__PoseEdge__Sequence *)(untyped_member);
  return &member->data[index];
}

void * slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_function__PoseGraph__edges(
  void * untyped_member, size_t index)
{
  slam_msgs__msg__PoseEdge__Sequence * member =
    (slam_msgs__msg__PoseEdge__Sequence *)(untyped_member);
  return &member->data[index];
}

void slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__fetch_function__PoseGraph__edges(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const slam_msgs__msg__PoseEdge * item =
    ((const slam_msgs__msg__PoseEdge *)
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_const_function__PoseGraph__edges(untyped_member, index));
  slam_msgs__msg__PoseEdge * value =
    (slam_msgs__msg__PoseEdge *)(untyped_value);
  *value = *item;
}

void slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__assign_function__PoseGraph__edges(
  void * untyped_member, size_t index, const void * untyped_value)
{
  slam_msgs__msg__PoseEdge * item =
    ((slam_msgs__msg__PoseEdge *)
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_function__PoseGraph__edges(untyped_member, index));
  const slam_msgs__msg__PoseEdge * value =
    (const slam_msgs__msg__PoseEdge *)(untyped_value);
  *item = *value;
}

bool slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__resize_function__PoseGraph__edges(
  void * untyped_member, size_t size)
{
  slam_msgs__msg__PoseEdge__Sequence * member =
    (slam_msgs__msg__PoseEdge__Sequence *)(untyped_member);
  slam_msgs__msg__PoseEdge__Sequence__fini(member);
  return slam_msgs__msg__PoseEdge__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_member_array[4] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseGraph, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "node_poses",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseGraph, node_poses),  // bytes offset in struct
    NULL,  // default value
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__size_function__PoseGraph__node_poses,  // size() function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_const_function__PoseGraph__node_poses,  // get_const(index) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_function__PoseGraph__node_poses,  // get(index) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__fetch_function__PoseGraph__node_poses,  // fetch(index, &value) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__assign_function__PoseGraph__node_poses,  // assign(index, value) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__resize_function__PoseGraph__node_poses  // resize(index) function pointer
  },
  {
    "num_nodes",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseGraph, num_nodes),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "edges",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(slam_msgs__msg__PoseGraph, edges),  // bytes offset in struct
    NULL,  // default value
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__size_function__PoseGraph__edges,  // size() function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_const_function__PoseGraph__edges,  // get_const(index) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__get_function__PoseGraph__edges,  // get(index) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__fetch_function__PoseGraph__edges,  // fetch(index, &value) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__assign_function__PoseGraph__edges,  // assign(index, value) function pointer
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__resize_function__PoseGraph__edges  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_members = {
  "slam_msgs__msg",  // message namespace
  "PoseGraph",  // message name
  4,  // number of fields
  sizeof(slam_msgs__msg__PoseGraph),
  slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_member_array,  // message members
  slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_init_function,  // function to initialize message memory (memory has to be allocated)
  slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_type_support_handle = {
  0,
  &slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_slam_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, slam_msgs, msg, PoseGraph)() {
  slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, slam_msgs, msg, PoseEdge)();
  if (!slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_type_support_handle.typesupport_identifier) {
    slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &slam_msgs__msg__PoseGraph__rosidl_typesupport_introspection_c__PoseGraph_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
