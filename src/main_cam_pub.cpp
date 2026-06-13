#include "cam_pub/cam_pub.hpp"

// Main function
int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CamPub>();

    while (rclcpp::ok() && node->is_publishing())
    {
        rclcpp::spin_some(node);
    }

    rclcpp::shutdown();
    return 0;
}