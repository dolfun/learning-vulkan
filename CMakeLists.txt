cmake_minimum_required(VERSION 3.24.0)
project(learning-vulkan VERSION 1.0)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(src)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/resources)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/texture.jpg
    ${CMAKE_CURRENT_BINARY_DIR}/resources/texture.jpg
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/viking_room.obj
    ${CMAKE_CURRENT_BINARY_DIR}/resources/viking_room.obj
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/viking_room.png
    ${CMAKE_CURRENT_BINARY_DIR}/resources/viking_room.png
    COPYONLY
)