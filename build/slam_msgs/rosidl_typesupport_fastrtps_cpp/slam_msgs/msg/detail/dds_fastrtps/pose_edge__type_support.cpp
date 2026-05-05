// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice
#include "slam_msgs/msg/detail/pose_edge__rosidl_typesupport_fastrtps_cpp.hpp"
#include "slam_msgs/msg/detail/pose_edge__struct.hpp"

#include <limits>
#include <stdexcept>
#include <string>
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_fastrtps_cpp/identifier.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"
#include "fastcdr/Cdr.h"


// forward declaration of message dependencies and their conversion functions

namespace slam_msgs
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_slam_msgs
cdr_serialize(
  const slam_msgs::msg::PoseEdge & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: from_id
  cdr << ros_message.from_id;
  // Member: to_id
  cdr << ros_message.to_id;
  // Member: z
  {
    cdr << ros_message.z;
  }
  // Member: omega
  {
    cdr << ros_message.omega;
  }
  // Member: edge_type
  cdr << ros_message.edge_type;
  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_slam_msgs
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  slam_msgs::msg::PoseEdge & ros_message)
{
  // Member: from_id
  cdr >> ros_message.from_id;

  // Member: to_id
  cdr >> ros_message.to_id;

  // Member: z
  {
    cdr >> ros_message.z;
  }

  // Member: omega
  {
    cdr >> ros_message.omega;
  }

  // Member: edge_type
  cdr >> ros_message.edge_type;

  return true;
}  // NOLINT(readability/fn_size)

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_slam_msgs
get_serialized_size(
  const slam_msgs::msg::PoseEdge & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: from_id
  {
    size_t item_size = sizeof(ros_message.from_id);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: to_id
  {
    size_t item_size = sizeof(ros_message.to_id);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: z
  {
    size_t array_size = 6;
    size_t item_size = sizeof(ros_message.z[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: omega
  {
    size_t array_size = 9;
    size_t item_size = sizeof(ros_message.omega[0]);
    current_alignment += array_size * item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: edge_type
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message.edge_type.size() + 1);

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_slam_msgs
max_serialized_size_PoseEdge(
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


  // Member: from_id
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: to_id
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: z
  {
    size_t array_size = 6;

    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: omega
  {
    size_t array_size = 9;

    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: edge_type
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
    using DataType = slam_msgs::msg::PoseEdge;
    is_plain =
      (
      offsetof(DataType, edge_type) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static bool _PoseEdge__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  auto typed_message =
    static_cast<const slam_msgs::msg::PoseEdge *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _PoseEdge__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<slam_msgs::msg::PoseEdge *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _PoseEdge__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const slam_msgs::msg::PoseEdge *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _PoseEdge__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_PoseEdge(full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}

static message_type_support_callbacks_t _PoseEdge__callbacks = {
  "slam_msgs::msg",
  "PoseEdge",
  _PoseEdge__cdr_serialize,
  _PoseEdge__cdr_deserialize,
  _PoseEdge__get_serialized_size,
  _PoseEdge__max_serialized_size
};

static rosidl_message_type_support_t _PoseEdge__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_PoseEdge__callbacks,
  get_message_typesupport_handle_function,
};

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace slam_msgs

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_slam_msgs
const rosidl_message_type_support_t *
get_message_type_support_handle<slam_msgs::msg::PoseEdge>()
{
  return &slam_msgs::msg::typesupport_fastrtps_cpp::_PoseEdge__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, slam_msgs, msg, PoseEdge)() {
  return &slam_msgs::msg::typesupport_fastrtps_cpp::_PoseEdge__handle;
}

#ifdef __cplusplus
}
#endif
