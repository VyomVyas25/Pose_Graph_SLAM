// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from slam_msgs:msg/PoseGraph.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__STRUCT_H_
#define SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'node_poses'
#include "rosidl_runtime_c/primitives_sequence.h"
// Member 'edges'
#include "slam_msgs/msg/detail/pose_edge__struct.h"

/// Struct defined in msg/PoseGraph in the package slam_msgs.
typedef struct slam_msgs__msg__PoseGraph
{
  std_msgs__msg__Header header;
  rosidl_runtime_c__double__Sequence node_poses;
  int32_t num_nodes;
  slam_msgs__msg__PoseEdge__Sequence edges;
} slam_msgs__msg__PoseGraph;

// Struct for a sequence of slam_msgs__msg__PoseGraph.
typedef struct slam_msgs__msg__PoseGraph__Sequence
{
  slam_msgs__msg__PoseGraph * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} slam_msgs__msg__PoseGraph__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__STRUCT_H_
