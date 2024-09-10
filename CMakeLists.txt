cmake_minimum_required(VERSION 3.28)

# project name and version
project(RayTracingInOneWeekend LANGUAGES CXX VERSION 1.0)

# set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# libigl options (since you're not using OpenGL for real-time rendering, we disable it)
set(LIBIGL_WITH_OPENGL OFF)               # Disable OpenGL features of libigl
set(LIBIGL_WITH_OPENGL_GLFW OFF)          # Disable OpenGL with GLFW in libigl
set(LIBIGL_WITH_VIEWER OFF)               # Disable libigl viewer (which depends on OpenGL)

# add subdirectory for libigl
add_subdirectory(ext/libigl)

# Find OpenMP package
find_package(OpenMP REQUIRED)

# Find Eigen3 package
find_package(Eigen3 3.4 REQUIRED NO_MODULE)

# include src directory so no need to change #include paths
include_directories(${CMAKE_SOURCE_DIR}/src)

# add executable with source files in the src directory
add_executable(raytracing src/main.cpp)

# link libigl to your raytracing executable
target_link_libraries(raytracing PUBLIC igl::core)

# include directories for libigl headers
target_include_directories(raytracing PUBLIC ext/libigl/include /usr/include/eigen3/)

# Link OpenMP libraries to the project
target_link_libraries(raytracing PUBLIC OpenMP::OpenMP_CXX)
