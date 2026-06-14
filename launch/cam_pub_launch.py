from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution


def generate_launch_description():
    default_param_file = PathJoinSubstitution(
        [get_package_share_directory("cam_pub"), "config", "webcam.yaml"]
    )

    namespace = LaunchConfiguration("namespace", default="")
    threaded = LaunchConfiguration("threaded", default="false")
    param_file_path = LaunchConfiguration("param_file_path", default=default_param_file)

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "namespace",
                default_value="",
                description="ROS namespace for the cam_pub node",
            ),
            DeclareLaunchArgument(
                "threaded",
                default_value="false",
                description="Whether to use the threaded version of the node",
            ),
            DeclareLaunchArgument(
                "param_file_path",
                default_value=default_param_file,
                description="Path to the parameters YAML file",
            ),
            Node(
                package="cam_pub",
                executable="cam_pub_node",
                name="cam_pub",
                namespace=namespace,
                parameters=[param_file_path],
                output="screen",
                condition=UnlessCondition(threaded),
            ),
            Node(
                package="cam_pub",
                executable="cam_pub_threaded_node",
                name="cam_pub",
                namespace=namespace,
                parameters=[param_file_path],
                output="screen",
                condition=IfCondition(threaded),
            ),
        ]
    )
