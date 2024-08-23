cmake_minimum_required(VERSION 3.28)

# 项目名称和版本
project(RayTracingInOneWeekend LANGUAGES CXX VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加可执行文件
add_executable(raytracing main.cpp)
