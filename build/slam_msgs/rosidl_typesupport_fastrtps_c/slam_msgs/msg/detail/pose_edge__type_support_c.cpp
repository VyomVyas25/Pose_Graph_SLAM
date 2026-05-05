// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice
#include "slam_msgs/msg/detail/pose_edge__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "slam_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "slam_msgs/msg/detail/pose_edge__struct.h"
#include "slam_msgs/msg/detail/pose_edge__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "rosidl_runtime_c/string.h"  // edge_type
#include "rosidl_runtime_c/string_functions.h"  // edge_type

// forward declare type support functions


using _PoseEdge__ros_msg_type = slam_msgs__msg__PoseEdge;

static bool _PoseEdge__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _PoseEdge__ros_msg_type * ros_message = static_cast<const _PoseEdge__ros_msg_type *>(untyped_ros_message);
  // Field name: from_id
  {
    cdr << ros_message->from_id;
  }

  // Field name: to_id
  {
    cdr << ros_message->to_id;
  }

  // Field name: z
  {
    size_t size = 6;
    auto array_ptr = ros_message->z;
    cdr.serializeArray(array_ptr, size);
  }

  // Field name: omega
  {
    size_t size = 9;
    auto array_ptr = ros_message->omega;
    cdr.serializeArray(array_ptr, size);
  }

  // Field name: edge_type
  {
    const rosidl_runtime_c__String * str = &ros_message->edge_type;
    if (str->capacity == 0 || str->capacity <= str->size) {
      fprintf(stderr, "string capacity not greater than size\n");
      return false;
    }
    if (str->data[str->size] != '\0') {
      fprintf(stderr, "string not null-terminated\n");
      return false;
    }
    cdr << str->data;
  }

  return true;
}

static bool _PoseEdge__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _PoseEdge__ros_msg_type * ros_message = static_cast<_PoseEdge__ros_msg_type *>(untyped_ros_message);
  // Field name: from_id
  {
    cdr >> ros_message->from_id;
  }

  // Field name: to_id
  {
    cdr >> ros_message->to_id;
  }

  // Field name: z
  {
    size_t size = 6;
    auto array_ptr = ros_message->z;
    cdr.deserializeArray(array_ptr, size);
  }

  // Field name: omega
  {
    size_t size = 9;
    auto array_ptr = ros_message->omega;
    cdr.deserializeArray(array_ptr, size);
  }

  // Field name: edge_type
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->edge_type.data) {
      rosidl_runtime_c__String__init(&ros_message->edge_type);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->edge_type,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'edge_type'\n");
      return false;
    }
  }

  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_slam_msgs
size_t get_serialized_size_slam_msgs__msg__PoseEdge(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _PoseEdge__ros_msg_type * ros_message = static_cast<const _PoseEdge__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name from_id
  {
    size_t item_size = sizeof(ros_message->from_id);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name to_id
  {
    size_t item_size = sizeof(ros_message->to_id);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name z
  {
    size_t array_size = 6;
    auto array_ptr = ros_message->z;
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name omega
  {
    size_t array_size = 9;
    auto array_ptr = ros_message->omega;
    (void)array_ptr;
    size_t item_size = sizeof(array_ptr[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name edge_type
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->edge_type.size + 1);

  return current_alignment - initial_alignment;
}

static uint32_t _PoseEdge__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_slam_msgs__msg__PoseEdge(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_slam_msgs
size_t max_serialized_size_slam_msgs__msg__PoseEdge(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // member: from_id
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: to_id
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: z
  {
    size_t array_size = 6;

    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }
  // member: omega
  {
    size_t array_size = 9;

    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }
  // member: edge_type
  {
    size_t array_size = 1;

    full_bounded = false;
    is_plain = false;
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
        1;
    }
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = slam_msgs__msg__PoseEdge;
    is_plain =
      (
      offsetof(DataType, edge_type) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static size_t _PoseEdge__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_slam_msgs__msg__PoseEdge(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_PoseEdge = {
  "slam_msgs::msg",
  "PoseEdge",
  _PoseEdge__cdr_serialize,
  _PoseEdge__cdr_deserialize,
  _PoseEdge__get_serialized_size,
  _PoseEdge__max_serialized_size
};

static rosidl_message_type_support_t _PoseEdge__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_PoseEdge,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, slam_msgs, msg, PoseEdge)() {
  return &_PoseEdge__type_support;
}

#if defined(__cplusplus)
}
#endif
