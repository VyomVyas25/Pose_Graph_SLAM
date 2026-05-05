// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from slam_msgs:msg/PoseGraph.idl
// generated code does not contain a copyright notice
#include "slam_msgs/msg/detail/pose_graph__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `node_poses`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `edges`
#include "slam_msgs/msg/detail/pose_edge__functions.h"

bool
slam_msgs__msg__PoseGraph__init(slam_msgs__msg__PoseGraph * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    slam_msgs__msg__PoseGraph__fini(msg);
    return false;
  }
  // node_poses
  if (!rosidl_runtime_c__double__Sequence__init(&msg->node_poses, 0)) {
    slam_msgs__msg__PoseGraph__fini(msg);
    return false;
  }
  // num_nodes
  // edges
  if (!slam_msgs__msg__PoseEdge__Sequence__init(&msg->edges, 0)) {
    slam_msgs__msg__PoseGraph__fini(msg);
    return false;
  }
  return true;
}

void
slam_msgs__msg__PoseGraph__fini(slam_msgs__msg__PoseGraph * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // node_poses
  rosidl_runtime_c__double__Sequence__fini(&msg->node_poses);
  // num_nodes
  // edges
  slam_msgs__msg__PoseEdge__Sequence__fini(&msg->edges);
}

bool
slam_msgs__msg__PoseGraph__are_equal(const slam_msgs__msg__PoseGraph * lhs, const slam_msgs__msg__PoseGraph * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // node_poses
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->node_poses), &(rhs->node_poses)))
  {
    return false;
  }
  // num_nodes
  if (lhs->num_nodes != rhs->num_nodes) {
    return false;
  }
  // edges
  if (!slam_msgs__msg__PoseEdge__Sequence__are_equal(
      &(lhs->edges), &(rhs->edges)))
  {
    return false;
  }
  return true;
}

bool
slam_msgs__msg__PoseGraph__copy(
  const slam_msgs__msg__PoseGraph * input,
  slam_msgs__msg__PoseGraph * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // node_poses
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->node_poses), &(output->node_poses)))
  {
    return false;
  }
  // num_nodes
  output->num_nodes = input->num_nodes;
  // edges
  if (!slam_msgs__msg__PoseEdge__Sequence__copy(
      &(input->edges), &(output->edges)))
  {
    return false;
  }
  return true;
}

slam_msgs__msg__PoseGraph *
slam_msgs__msg__PoseGraph__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  slam_msgs__msg__PoseGraph * msg = (slam_msgs__msg__PoseGraph *)allocator.allocate(sizeof(slam_msgs__msg__PoseGraph), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(slam_msgs__msg__PoseGraph));
  bool success = slam_msgs__msg__PoseGraph__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
slam_msgs__msg__PoseGraph__destroy(slam_msgs__msg__PoseGraph * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    slam_msgs__msg__PoseGraph__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
slam_msgs__msg__PoseGraph__Sequence__init(slam_msgs__msg__PoseGraph__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  slam_msgs__msg__PoseGraph * data = NULL;

  if (size) {
    data = (slam_msgs__msg__PoseGraph *)allocator.zero_allocate(size, sizeof(slam_msgs__msg__PoseGraph), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = slam_msgs__msg__PoseGraph__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        slam_msgs__msg__PoseGraph__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
slam_msgs__msg__PoseGraph__Sequence__fini(slam_msgs__msg__PoseGraph__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      slam_msgs__msg__PoseGraph__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

slam_msgs__msg__PoseGraph__Sequence *
slam_msgs__msg__PoseGraph__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  slam_msgs__msg__PoseGraph__Sequence * array = (slam_msgs__msg__PoseGraph__Sequence *)allocator.allocate(sizeof(slam_msgs__msg__PoseGraph__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = slam_msgs__msg__PoseGraph__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
slam_msgs__msg__PoseGraph__Sequence__destroy(slam_msgs__msg__PoseGraph__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    slam_msgs__msg__PoseGraph__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
slam_msgs__msg__PoseGraph__Sequence__are_equal(const slam_msgs__msg__PoseGraph__Sequence * lhs, const slam_msgs__msg__PoseGraph__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!slam_msgs__msg__PoseGraph__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
slam_msgs__msg__PoseGraph__Sequence__copy(
  const slam_msgs__msg__PoseGraph__Sequence * input,
  slam_msgs__msg__PoseGraph__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(slam_msgs__msg__PoseGraph);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    slam_msgs__msg__PoseGraph * data =
      (slam_msgs__msg__PoseGraph *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!slam_msgs__msg__PoseGraph__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          slam_msgs__msg__PoseGraph__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!slam_msgs__msg__PoseGraph__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
