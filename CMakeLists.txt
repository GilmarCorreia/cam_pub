cmake_minimum_required(VERSION 3.8)
project(cam_pub)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rosidl_default_runtime REQUIRED)

# Generate messages
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/CamStatus.msg"
  DEPENDENCIES std_msgs
)
ament_export_dependencies(rosidl_default_runtime)
rosidl_get_typesupport_target(
  cpp_typesupport_target
  "${PROJECT_NAME}"
  "rosidl_typesupport_cpp"
)

# Base lib
add_library(cam_pub_lib STATIC
  src/cam_pub.cpp
)

target_include_directories(cam_pub_lib PUBLIC include)

ament_target_dependencies(
  cam_pub_lib
  rclcpp
  sensor_msgs
  cv_bridge
  image_transport
)

target_link_libraries(
  cam_pub_lib
  ${OpenCV_LIBS}
  ${cpp_typesupport_target}
)

# Add executable
add_executable(cam_pub src/main_cam_pub.cpp)

target_link_libraries(
  cam_pub
  cam_pub_lib
)

# Add threaded test executable
add_executable(
  cam_pub_threaded 
  src/cam_pub_threaded.cpp
  src/main_cam_pub_threaded.cpp
)
target_link_libraries(
  cam_pub_threaded
  cam_pub_lib
)

install(TARGETS
  cam_pub
  cam_pub_threaded
  cam_pub_lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

# Install launch and config directories
install(
  DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
