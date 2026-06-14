#include "cam_pub/cam_pub.hpp"

// Constructor
CamPub::CamPub(bool init) : Node("cam_pub") {
  // Declare parameters with default values
  this->declare_parameter<std::string>("camera_name", "camera");
  this->declare_parameter<std::string>("camera_topic", "/image_raw");
  this->declare_parameter<std::string>("camera_status_topic", "/status");
  this->declare_parameter<std::vector<int64_t>>("image_size",
                                                std::vector<int64_t>{640, 480});
  this->declare_parameter<int>("publish_rate", 10);
  this->declare_parameter<std::string>("device_path", "/dev/video0");

  // Use parameters to set the attributes of the node
  this->set_camera_name(this->get_parameter("camera_name").as_string());
  this->set_camera_topic(this->get_camera_name() +
                         this->get_parameter("camera_topic").as_string());
  this->set_camera_status_topic(
      this->get_camera_name() +
      this->get_parameter("camera_status_topic").as_string());
  this->set_image_size(this->get_parameter("image_size").as_integer_array());
  this->set_publish_rate(this->get_parameter("publish_rate").as_int());
  this->set_device_path(this->get_parameter("device_path").as_string());

  // Try to open the camera device
  try {
    RCLCPP_INFO(this->get_logger(), "Initializing camera at %s",
                this->get_device_path().c_str());

    // Open video capture
    this->cap_.open(this->get_device_path());
    if (!this->cap_.isOpened()) {
      RCLCPP_ERROR(this->get_logger(), "Cannot open camera at %s",
                   this->get_device_path().c_str());
      this->set_is_publishing(false);
    } else {
      // Set image size
      this->cap_.set(cv::CAP_PROP_FRAME_WIDTH,
                     static_cast<double>(this->get_image_size()[0]));
      this->cap_.set(cv::CAP_PROP_FRAME_HEIGHT,
                     static_cast<double>(this->get_image_size()[1]));
      RCLCPP_INFO(this->get_logger(), "Camera opened: %s",
                  this->get_device_path().c_str());
      this->set_is_publishing(true);
    }
  } catch (const std::exception &e) {
    RCLCPP_ERROR(this->get_logger(), "Failed to initialize camera: %s",
                 e.what());
    this->set_is_publishing(false);
  }

  if (init) {
    this->initialize();
  }
}

// Setters
void CamPub::set_camera_name(const std::string &camera_name) {
  this->cam_name_ = camera_name;
}

void CamPub::set_camera_topic(const std::string &camera_topic) {
  this->camera_topic_ = camera_topic;
}

void CamPub::set_camera_status_topic(const std::string &camera_status_topic) {
  this->status_topic_ = camera_status_topic;
}

void CamPub::set_image_size(const std::vector<int64_t> &image_size) {
  this->image_size_ = image_size;
}

void CamPub::set_publish_rate(int publish_rate) {
  this->publish_rate_ = publish_rate;
}

void CamPub::set_device_path(const std::string &device_path) {
  this->device_path_ = device_path;
}

void CamPub::set_is_publishing(bool is_publishing) {
  this->is_publishing_ = is_publishing;
}

// Getters
const std::string &CamPub::get_camera_name() const { return this->cam_name_; }

const std::string &CamPub::get_camera_topic() const {
  return this->camera_topic_;
}

const std::string &CamPub::get_camera_status_topic() const {
  return this->status_topic_;
}

const std::vector<int64_t> &CamPub::get_image_size() const {
  return this->image_size_;
}

int CamPub::get_publish_rate() const { return this->publish_rate_; }

const std::string &CamPub::get_device_path() const {
  return this->device_path_;
}

bool CamPub::is_publishing() const { return this->is_publishing_; }

// Methods
bool CamPub::capture_frame(cv::Mat &frame) { return cap_.read(frame); }

void CamPub::initialize() {
  // Publishers
  this->campub_ = this->create_publisher<sensor_msgs::msg::Image>(
      this->get_camera_topic(), rclcpp::SensorDataQoS());
  this->status_pub_ =
      this->create_publisher<challenge_interfaces::msg::CamStatus>(
          this->get_camera_status_topic(), rclcpp::QoS(10).reliable());

  // Timers
  auto fps = 1000ms / this->get_publish_rate();
  this->timer_ =
      this->create_wall_timer(fps, std::bind(&CamPub::timer_callback, this));
}

// Callbacks
void CamPub::timer_callback() {
  auto message = sensor_msgs::msg::Image();
  auto status_message = challenge_interfaces::msg::CamStatus();

  status_message.header.stamp = this->get_clock()->now();

  try {
    // Capture frame if camera is opened
    if (this->is_publishing()) {
      cv::Mat frame;
      if (this->capture_frame(frame) && !frame.empty()) {
        std_msgs::msg::Header header;
        header.stamp = this->get_clock()->now();
        auto img_msg = cv_bridge::CvImage(header, "bgr8", frame).toImageMsg();
        this->campub_->publish(*img_msg);
        this->images_sent_++;
      } else {
        RCLCPP_WARN(this->get_logger(), "No frame captured from camera");
        this->set_is_publishing(false);
      }
    }
  } catch (const std::exception &e) {
    RCLCPP_ERROR(this->get_logger(), "Failed to publish image: %s", e.what());
    this->set_is_publishing(false);
  }

  status_message.active = this->is_publishing();
  status_message.counter = this->images_sent_;
  this->status_pub_->publish(status_message);
}
