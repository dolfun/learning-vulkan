cmake_minimum_required(VERSION 3.20.0)
project(learning-vulkan VERSION 1.0)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(main 
                src/main.cc
                src/application.h src/application.cc
                src/debug_messenger.h
                src/utility.h)
target_link_libraries(main PRIVATE glfw Vulkan::Vulkan)
target_compile_features(main PRIVATE cxx_std_17)