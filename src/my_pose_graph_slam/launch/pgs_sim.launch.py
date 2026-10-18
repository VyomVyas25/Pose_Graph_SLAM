"""Gazebo pgs_world + TurtleBot3 waffle + the pose graph SLAM + RViz + a scripted loop route.

    ros2 launch my_pose_graph_slam pgs_sim.launch.py              # noise on, 3 laps
    ros2 launch my_pose_graph_slam pgs_sim.launch.py noise:=false laps:=2 drive:=false

noise:=true drifts the odometry the SLAM sees (increments perturbed inside graph_builder_node),
so /odom_path (red) drifts away from /gt_path (blue) and the loop closures pull /optimized_path
(green) and the map back. Needs turtlebot3_gazebo (with worlds/pgs_world.world) sourced.
"""
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, IncludeLaunchDescription, SetEnvironmentVariable,
                            TimerAction, UnsetEnvironmentVariable)
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg = get_package_share_directory('my_pose_graph_slam')
    tb3 = get_package_share_directory('turtlebot3_gazebo')
    gz = get_package_share_directory('gazebo_ros')
    world = os.path.join(tb3, 'worlds', 'pgs_world.world')
    return LaunchDescription([
        DeclareLaunchArgument('noise', default_value='true'),
        DeclareLaunchArgument('laps', default_value='3'),
        DeclareLaunchArgument('drive', default_value='true'),
        DeclareLaunchArgument('gui', default_value='true'),
        SetEnvironmentVariable('TURTLEBOT3_MODEL', 'waffle'),
        # a terminal opened from VS Code's snap leaks these and RViz / gzclient crash
        *[UnsetEnvironmentVariable(v) for v in ('GTK_PATH', 'GTK_EXE_PREFIX', 'GIO_MODULE_DIR', 'GTK_IM_MODULE_FILE', 'LOCPATH',
                                                'GDK_PIXBUF_MODULE_FILE', 'GDK_PIXBUF_MODULEDIR', 'GSETTINGS_SCHEMA_DIR')],
        IncludeLaunchDescription(PythonLaunchDescriptionSource(os.path.join(gz, 'launch', 'gzserver.launch.py')),
                                 launch_arguments={'world': world}.items()),
        IncludeLaunchDescription(PythonLaunchDescriptionSource(os.path.join(gz, 'launch', 'gzclient.launch.py')),
                                 condition=IfCondition(LaunchConfiguration('gui'))),
        IncludeLaunchDescription(PythonLaunchDescriptionSource(os.path.join(tb3, 'launch', 'robot_state_publisher.launch.py')),
                                 launch_arguments={'use_sim_time': 'true'}.items()),
        IncludeLaunchDescription(PythonLaunchDescriptionSource(os.path.join(tb3, 'launch', 'spawn_turtlebot3.launch.py')),
                                 launch_arguments={'x_pose': '0.0', 'y_pose': '0.0'}.items()),
        IncludeLaunchDescription(PythonLaunchDescriptionSource(os.path.join(pkg, 'launch', 'slam.launch.py')),
                                 launch_arguments={'use_sim_time': 'true', 'noise': LaunchConfiguration('noise')}.items()),
        TimerAction(period=10.0, actions=[
            Node(package='my_pose_graph_slam', executable='drive_loop.py', output='screen',
                 parameters=[{'use_sim_time': True, 'laps': LaunchConfiguration('laps')}],
                 condition=IfCondition(LaunchConfiguration('drive')))]),
    ])
