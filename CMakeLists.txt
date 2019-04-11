cmake_minimum_required(VERSION 2.8)

project(OpenCV-Webcam)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(src)