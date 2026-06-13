#pragma once

#include "cam_pub/cam_pub.hpp"

#include <atomic>
#include <mutex>
#include <thread>

class CamPubThreaded : public CamPub
{
public:
    CamPubThreaded();
    virtual ~CamPubThreaded();

protected:
    bool capture_frame(cv::Mat &frame) override;

private:
    void capture_loop();

    std::thread capture_thread_;

    std::mutex frame_mutex_;

    cv::Mat latest_frame_;

    std::atomic<bool> running_{true};
    std::atomic<bool> frame_available_{false};
};