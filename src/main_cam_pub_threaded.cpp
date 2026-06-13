#include "object_tracker/cam_pub_threaded.hpp"

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CamPubThreaded>();

  while (rclcpp::ok() && node->is_publishing())
  {
    rclcpp::spin_some(node);
  }

  rclcpp::shutdown();
  return 0;
}