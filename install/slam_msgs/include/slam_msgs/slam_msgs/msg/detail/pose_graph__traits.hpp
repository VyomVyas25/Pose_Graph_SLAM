// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from slam_msgs:msg/PoseGraph.idl
// generated code does not contain a copyright notice

#ifndef SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__TRAITS_HPP_
#define SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "slam_msgs/msg/detail/pose_graph__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'edges'
#include "slam_msgs/msg/detail/pose_edge__traits.hpp"

namespace slam_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const PoseGraph & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: node_poses
  {
    if (msg.node_poses.size() == 0) {
      out << "node_poses: []";
    } else {
      out << "node_poses: [";
      size_t pending_items = msg.node_poses.size();
      for (auto item : msg.node_poses) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: num_nodes
  {
    out << "num_nodes: ";
    rosidl_generator_traits::value_to_yaml(msg.num_nodes, out);
    out << ", ";
  }

  // member: edges
  {
    if (msg.edges.size() == 0) {
      out << "edges: []";
    } else {
      out << "edges: [";
      size_t pending_items = msg.edges.size();
      for (auto item : msg.edges) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const PoseGraph & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: node_poses
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.node_poses.size() == 0) {
      out << "node_poses: []\n";
    } else {
      out << "node_poses:\n";
      for (auto item : msg.node_poses) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: num_nodes
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "num_nodes: ";
    rosidl_generator_traits::value_to_yaml(msg.num_nodes, out);
    out << "\n";
  }

  // member: edges
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.edges.size() == 0) {
      out << "edges: []\n";
    } else {
      out << "edges:\n";
      for (auto item : msg.edges) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const PoseGraph & msg, bool use_flow_style = false)
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
  const slam_msgs::msg::PoseGraph & msg,
  std::ostream & out, size_t indentation = 0)
{
  slam_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use slam_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const slam_msgs::msg::PoseGraph & msg)
{
  return slam_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<slam_msgs::msg::PoseGraph>()
{
  return "slam_msgs::msg::PoseGraph";
}

template<>
inline const char * name<slam_msgs::msg::PoseGraph>()
{
  return "slam_msgs/msg/PoseGraph";
}

template<>
struct has_fixed_size<slam_msgs::msg::PoseGraph>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<slam_msgs::msg::PoseGraph>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<slam_msgs::msg::PoseGraph>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SLAM_MSGS__MSG__DETAIL__POSE_GRAPH__TRAITS_HPP_
