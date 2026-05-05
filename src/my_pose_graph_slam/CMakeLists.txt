cmake_minimum_required(VERSION 3.8)
project(my_pose_graph_slam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(slam_msgs REQUIRED)
find_package(Eigen3 REQUIRED)

# Graph Builder
add_executable(graph_builder_node src/GraphBuilderNode.cpp)
ament_target_dependencies(graph_builder_node
  rclcpp sensor_msgs nav_msgs geometry_msgs std_msgs
  slam_msgs tf2 tf2_ros tf2_geometry_msgs)
target_link_libraries(graph_builder_node Eigen3::Eigen)

# Backend Optimizer
add_executable(backend_optimizer_node src/BackendOptimizer.cpp)
ament_target_dependencies(backend_optimizer_node
  rclcpp slam_msgs visualization_msgs nav_msgs geometry_msgs std_msgs)
target_link_libraries(backend_optimizer_node Eigen3::Eigen)

# Point Cloud Map Builder (Option C - no Bresenham)
add_executable(point_cloud_map_node src/PointCloudMapNode.cpp)
ament_target_dependencies(point_cloud_map_node
  rclcpp sensor_msgs nav_msgs tf2 tf2_ros tf2_geometry_msgs)
target_link_libraries(point_cloud_map_node Eigen3::Eigen)

install(TARGETS
  graph_builder_node
  backend_optimizer_node
  point_cloud_map_node
  DESTINATION lib/${PROJECT_NAME})

# Install launch files so ros2 launch can find them
install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch)

# Install RViz config (OPTIONAL — won't fail if rviz/ is empty)
install(DIRECTORY rviz/
  DESTINATION share/${PROJECT_NAME}/rviz
  OPTIONAL)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()