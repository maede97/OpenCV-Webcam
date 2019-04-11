cmake_minimum_required(VERSION 2.8)

add_executable(webcam main.cpp)

target_link_libraries(webcam ${OpenCV_LIBS})