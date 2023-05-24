cmake_minimum_required(VERSION 3.24.0)
project(learning-vulkan VERSION 1.0)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(src)