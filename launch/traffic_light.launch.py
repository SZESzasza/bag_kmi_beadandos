from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='bag_kmi_beadandos',
            executable='traffic_light_node',
            output='screen',
            parameters=[{
                'red_time': 5.0,
                'red_yellow_time': 1.5,
                'green_time': 5.0,
                'yellow_time': 2.0,
            }],
        ),
    ])