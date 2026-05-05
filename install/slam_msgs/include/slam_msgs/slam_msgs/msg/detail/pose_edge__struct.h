// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_EDGE__STRUCT_H_
#define SLAM_MSGS__MSG__DETAIL__POSE_EDGE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'edge_type'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/PoseEdge in the package slam_msgs.
typedef struct slam_msgs__msg__PoseEdge
{
  int32_t from_id;
  int32_t to_id;
  double z[6];
  double omega[9];
  rosidl_runtime_c__String edge_type;
} slam_msgs__msg__PoseEdge;

// Struct for a sequence of slam_msgs__msg__PoseEdge.
typedef struct slam_msgs__msg__PoseEdge__Sequence
{
  slam_msgs__msg__PoseEdge * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} slam_msgs__msg__PoseEdge__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_EDGE__STRUCT_H_
