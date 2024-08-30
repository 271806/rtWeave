cmake_minimum_required(VERSION 3.28)

# project name and version
project(RayTracingInOneWeekend LANGUAGES CXX VERSION 1.0)

# set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add executable
add_executable(raytracing main.cpp)
add_executable(estimate_halfway math/estimate_halfway.cpp)