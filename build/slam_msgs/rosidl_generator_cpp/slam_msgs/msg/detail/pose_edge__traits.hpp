// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from slam_msgs:msg/PoseEdge.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_EDGE__TRAITS_HPP_
#define SLAM_MSGS__MSG__DETAIL__POSE_EDGE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "slam_msgs/msg/detail/pose_edge__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace slam_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const PoseEdge & msg,
  std::ostream & out)
{
  out << "{";
  // member: from_id
  {
    out << "from_id: ";
    rosidl_generator_traits::value_to_yaml(msg.from_id, out);
    out << ", ";
  }

  // member: to_id
  {
    out << "to_id: ";
    rosidl_generator_traits::value_to_yaml(msg.to_id, out);
    out << ", ";
  }

  // member: z
  {
    if (msg.z.size() == 0) {
      out << "z: []";
    } else {
      out << "z: [";
      size_t pending_items = msg.z.size();
      for (auto item : msg.z) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: omega
  {
    if (msg.omega.size() == 0) {
      out << "omega: []";
    } else {
      out << "omega: [";
      size_t pending_items = msg.omega.size();
      for (auto item : msg.omega) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: edge_type
  {
    out << "edge_type: ";
    rosidl_generator_traits::value_to_yaml(msg.edge_type, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const PoseEdge & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: from_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "from_id: ";
    rosidl_generator_traits::value_to_yaml(msg.from_id, out);
    out << "\n";
  }

  // member: to_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "to_id: ";
    rosidl_generator_traits::value_to_yaml(msg.to_id, out);
    out << "\n";
  }

  // member: z
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.z.size() == 0) {
      out << "z: []\n";
    } else {
      out << "z:\n";
      for (auto item : msg.z) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: omega
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.omega.size() == 0) {
      out << "omega: []\n";
    } else {
      out << "omega:\n";
      for (auto item : msg.omega) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: edge_type
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "edge_type: ";
    rosidl_generator_traits::value_to_yaml(msg.edge_type, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const PoseEdge & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace slam_msgs

namespace rosidl_generator_traits
{

[[deprecated("use slam_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const slam_msgs::msg::PoseEdge & msg,
  std::ostream & out, size_t indentation = 0)
{
  slam_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use slam_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const slam_msgs::msg::PoseEdge & msg)
{
  return slam_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<slam_msgs::msg::PoseEdge>()
{
  return "slam_msgs::msg::PoseEdge";
}

template<>
inline const char * name<slam_msgs::msg::PoseEdge>()
{
  return "slam_msgs/msg/PoseEdge";
}

template<>
struct has_fixed_size<slam_msgs::msg::PoseEdge>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<slam_msgs::msg::PoseEdge>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<slam_msgs::msg::PoseEdge>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_EDGE__TRAITS_HPP_
