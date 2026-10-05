import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    rviz_config = os.path.join(
        get_package_share_directory('bag_kmi_beadandos'), 'rviz', 'traffic_light.rviz')
    return LaunchDescription([
        Node(
            package='bag_kmi_beadandos',
            executable='traffic_light_node',
            output='screen',
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            arguments=['-d', rviz_config],
        ),
    ])