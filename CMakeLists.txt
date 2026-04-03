cmake_minimum_required(VERSION 3.5)
project(April LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(simple_logger REQUIRED)
add_executable(April main.cpp)

target_link_libraries(April PRIVATE simple_logger::simple_logger)
