// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice
#include "slam_msgs/msg/detail/pose_edge__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `edge_type`
#include "rosidl_runtime_c/string_functions.h"

bool
slam_msgs__msg__PoseEdge__init(slam_msgs__msg__PoseEdge * msg)
{
  if (!msg) {
    return false;
  }
  // from_id
  // to_id
  // z
  // omega
  // edge_type
  if (!rosidl_runtime_c__String__init(&msg->edge_type)) {
    slam_msgs__msg__PoseEdge__fini(msg);
    return false;
  }
  return true;
}

void
slam_msgs__msg__PoseEdge__fini(slam_msgs__msg__PoseEdge * msg)
{
  if (!msg) {
    return;
  }
  // from_id
  // to_id
  // z
  // omega
  // edge_type
  rosidl_runtime_c__String__fini(&msg->edge_type);
}

bool
slam_msgs__msg__PoseEdge__are_equal(const slam_msgs__msg__PoseEdge * lhs, const slam_msgs__msg__PoseEdge * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // from_id
  if (lhs->from_id != rhs->from_id) {
    return false;
  }
  // to_id
  if (lhs->to_id != rhs->to_id) {
    return false;
  }
  // z
  for (size_t i = 0; i < 6; ++i) {
    if (lhs->z[i] != rhs->z[i]) {
      return false;
    }
  }
  // omega
  for (size_t i = 0; i < 9; ++i) {
    if (lhs->omega[i] != rhs->omega[i]) {
      return false;
    }
  }
  // edge_type
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->edge_type), &(rhs->edge_type)))
  {
    return false;
  }
  return true;
}

bool
slam_msgs__msg__PoseEdge__copy(
  const slam_msgs__msg__PoseEdge * input,
  slam_msgs__msg__PoseEdge * output)
{
  if (!input || !output) {
    return false;
  }
  // from_id
  output->from_id = input->from_id;
  // to_id
  output->to_id = input->to_id;
  // z
  for (size_t i = 0; i < 6; ++i) {
    output->z[i] = input->z[i];
  }
  // omega
  for (size_t i = 0; i < 9; ++i) {
    output->omega[i] = input->omega[i];
  }
  // edge_type
  if (!rosidl_runtime_c__String__copy(
      &(input->edge_type), &(output->edge_type)))
  {
    return false;
  }
  return true;
}

slam_msgs__msg__PoseEdge *
slam_msgs__msg__PoseEdge__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  slam_msgs__msg__PoseEdge * msg = (slam_msgs__msg__PoseEdge *)allocator.allocate(sizeof(slam_msgs__msg__PoseEdge), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(slam_msgs__msg__PoseEdge));
  bool success = slam_msgs__msg__PoseEdge__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
slam_msgs__msg__PoseEdge__destroy(slam_msgs__msg__PoseEdge * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    slam_msgs__msg__PoseEdge__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
slam_msgs__msg__PoseEdge__Sequence__init(slam_msgs__msg__PoseEdge__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  slam_msgs__msg__PoseEdge * data = NULL;

  if (size) {
    data = (slam_msgs__msg__PoseEdge *)allocator.zero_allocate(size, sizeof(slam_msgs__msg__PoseEdge), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = slam_msgs__msg__PoseEdge__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        slam_msgs__msg__PoseEdge__fini(&data[i - 1]);
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
slam_msgs__msg__PoseEdge__Sequence__fini(slam_msgs__msg__PoseEdge__Sequence * array)
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
      slam_msgs__msg__PoseEdge__fini(&array->data[i]);
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

slam_msgs__msg__PoseEdge__Sequence *
slam_msgs__msg__PoseEdge__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  slam_msgs__msg__PoseEdge__Sequence * array = (slam_msgs__msg__PoseEdge__Sequence *)allocator.allocate(sizeof(slam_msgs__msg__PoseEdge__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = slam_msgs__msg__PoseEdge__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
slam_msgs__msg__PoseEdge__Sequence__destroy(slam_msgs__msg__PoseEdge__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    slam_msgs__msg__PoseEdge__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
slam_msgs__msg__PoseEdge__Sequence__are_equal(const slam_msgs__msg__PoseEdge__Sequence * lhs, const slam_msgs__msg__PoseEdge__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!slam_msgs__msg__PoseEdge__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
slam_msgs__msg__PoseEdge__Sequence__copy(
  const slam_msgs__msg__PoseEdge__Sequence * input,
  slam_msgs__msg__PoseEdge__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(slam_msgs__msg__PoseEdge);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    slam_msgs__msg__PoseEdge * data =
      (slam_msgs__msg__PoseEdge *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!slam_msgs__msg__PoseEdge__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          slam_msgs__msg__PoseEdge__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!slam_msgs__msg__PoseEdge__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
