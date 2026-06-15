# cam_pub

ROS 2 C++ package that captures webcam images and publishes the video stream and camera status.

Key features:
- Publishes images to `/camera/image_raw` as `sensor_msgs/Image`.
- Publishes status to `/camera/status` as `challenge_interfaces/CamStatus`.
- Supports the standard node (`cam_pub_node`) and threaded node (`cam_pub_threaded_node`).

This package is used by `challenge_bringup` to start video capture and provide camera data to the tracker.
