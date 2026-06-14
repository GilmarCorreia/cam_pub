// Imports
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#if __has_include(<cv_bridge/cv_bridge.hpp>)
#include <cv_bridge/cv_bridge.hpp>
#else
#include <cv_bridge/cv_bridge.h>
#endif

// ROS2 Imports
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "challenge_interfaces/msg/cam_status.hpp"

// Definitions
using namespace std::chrono_literals;

class CamPub : public rclcpp::Node
{
public:
    // Constructor
    CamPub(bool init = true);

    // Getters
    const std::string &get_camera_name() const;
    const std::string &get_camera_topic() const;
    const std::string &get_camera_status_topic() const;
    const std::vector<int64_t> &get_image_size() const;
    int get_publish_rate() const;
    const std::string &get_device_path() const;
    bool is_publishing() const;

protected:
    // Callbacks
    virtual void timer_callback();

    // Setters
    void set_is_publishing(bool is_publishing);

    // Methods
    virtual bool capture_frame(cv::Mat &frame);
    void initialize();

    // Publishers
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr campub_;
    rclcpp::Publisher<challenge_interfaces::msg::CamStatus>::SharedPtr status_pub_;

    // Timers
    rclcpp::TimerBase::SharedPtr timer_;

    // Attributes
    cv::VideoCapture cap_;
    bool is_publishing_;
    uint64_t images_sent_ = 0;

private:
    // Setters
    void set_camera_name(const std::string &camera_name);
    void set_camera_topic(const std::string &camera_topic);
    void set_camera_status_topic(const std::string &camera_status_topic);
    void set_image_size(const std::vector<int64_t> &image_size);
    void set_publish_rate(int publish_rate);
    void set_device_path(const std::string &device_path);

    // Attributes
    std::string cam_name_;
    std::string camera_topic_;
    std::string status_topic_;
    std::vector<int64_t> image_size_;
    int publish_rate_;
    std::string device_path_;
};
