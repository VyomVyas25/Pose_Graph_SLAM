import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, LogInfo
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    noise_arg = DeclareLaunchArgument(
        'noise',
        default_value='false',
        description='Add artificial noise to the odometry INCREMENTS (edges and initial guesses drift '
                    'like real odometry) to test scan matching and loop closure.')

    lc_cooldown_arg = DeclareLaunchArgument(
        'lc_cooldown',
        default_value='5',
        description='Minimum number of new nodes between two accepted loop closures. '
                    'Lower = denser LC coverage. Default 5.')

    noise_alpha1_arg = DeclareLaunchArgument(
        'noise_alpha1', default_value='0.02',
        description='Alpha-1: translation noise from translation (Thrun noise model).')

    noise_alpha3_arg = DeclareLaunchArgument(
        'noise_alpha3', default_value='0.005',
        description='Alpha-3: rotation noise from translation. '
                    '0.005 gives ~0.22 rad drift per lap — within ICP recovery range.')

    scan_skip_arg = DeclareLaunchArgument(
        'scan_skip', default_value='3',
        description='PointCloudMapNode: publish every N-th scan snapshot '
                    '(higher = fewer points, faster). Default 3.')

    use_rviz_arg = DeclareLaunchArgument(
        'rviz', default_value='true',
        description='Launch RViz2. Set to "false" to skip (e.g. headless runs).')

    sim_time_arg = DeclareLaunchArgument(
        'use_sim_time', default_value='false',
        description='true when running with Gazebo or a bag (--clock).')

    sim_time  = PythonExpression(["'", LaunchConfiguration('use_sim_time'), "' == 'true'"])
    noise     = LaunchConfiguration('noise')
    cooldown  = LaunchConfiguration('lc_cooldown')
    alpha1    = LaunchConfiguration('noise_alpha1')
    alpha3    = LaunchConfiguration('noise_alpha3')
    scan_skip = LaunchConfiguration('scan_skip')

    pkg_share = get_package_share_directory('my_pose_graph_slam')
    rviz_cfg  = os.path.join(pkg_share, 'rviz', 'slam.rviz')

    # map -> odom is published by graph_builder_node (newest node: optimised pose x odometry^-1)

    graph_builder = Node(
        package='my_pose_graph_slam',
        executable='graph_builder_node',
        name='graph_builder_node',
        output='screen',
        parameters=[{
            'odom_noise_enabled': PythonExpression(["'", noise, "' == 'true'"]),
            'noise_alpha1':  alpha1,
            'noise_alpha3':  alpha3,
            'lc_cooldown':   cooldown,
            'use_sim_time':  sim_time,
        }]
    )

    backend = Node(
        package='my_pose_graph_slam',
        executable='backend_optimizer_node',
        name='backend_optimizer_node',
        output='screen',
        parameters=[{'use_sim_time': sim_time}]
    )

    pcm = Node(
        package='my_pose_graph_slam',
        executable='point_cloud_map_node',
        name='point_cloud_map_node',
        output='screen',
        parameters=[{
            'scan_skip': scan_skip,
            'use_sim_time': sim_time,
        }]
    )

    rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_cfg] if os.path.exists(rviz_cfg) else [],
        output='screen',
        parameters=[{'use_sim_time': sim_time}],
        condition=IfCondition(LaunchConfiguration('rviz'))
    )

    return LaunchDescription([

        noise_arg,
        lc_cooldown_arg,
        noise_alpha1_arg,
        noise_alpha3_arg,
        scan_skip_arg,
        use_rviz_arg,
        sim_time_arg,

        LogInfo(msg=['[slam.launch] noise=', noise,
                     '  lc_cooldown=', cooldown,
                     '  alpha1=', alpha1,
                     '  alpha3=', alpha3]),

        graph_builder,
        backend,
        pcm,
        rviz,
    ])