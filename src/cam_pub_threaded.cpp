#include "object_tracker/cam_pub_threaded.hpp"

CamPubThreaded::CamPubThreaded() : CamPub(false)
{
    capture_thread_ = std::thread(&CamPubThreaded::capture_loop, this);

    RCLCPP_INFO(this->get_logger(), "Threaded camera capture enabled");

    // wait 2 seconds to allow camera to initialize
    std::this_thread::sleep_for(std::chrono::seconds(2));

    this->initialize();
}

CamPubThreaded::~CamPubThreaded()
{
    running_ = false;

    if (capture_thread_.joinable())
    {
        capture_thread_.join();
    }
}

void CamPubThreaded::capture_loop()
{
    while (running_ && is_publishing())
    {
        cv::Mat frame;

        if (cap_.read(frame))
        {
            std::lock_guard<std::mutex> lock(frame_mutex_);

            frame.copyTo(latest_frame_);

            frame_available_ = true;
        }
        else
        {
            RCLCPP_WARN(this->get_logger(), "Failed to capture frame");
        }
    }
}

bool CamPubThreaded::capture_frame(cv::Mat &frame)
{
    if (!frame_available_)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(frame_mutex_);

    if (latest_frame_.empty())
    {
        return false;
    }

    latest_frame_.copyTo(frame);

    return true;
}
