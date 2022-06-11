cmake_minimum_required(VERSION 3.10)

project(Ray_Tracing)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(utilities)

# add_library(Ray_Tracing main.cpp)
add_executable(Ray_Tracing main.cpp)

target_link_options(Ray_Tracing PRIVATE -pthread)

