cmake_minimum_required(VERSION 3.24.0)
project(learning-vulkan VERSION 1.0)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

message(STATUS "Looking for stb_image.h")
find_path(STB_INCLUDE_DIR stb_image.h)
if(NOT STB_INCLUDE_DIR)
    find_path(STB_INCLUDE_DIR NAMES stb_image.h PATH_SUFFIXES stb)
endif()
if(STB_INCLUDE_DIR)
    message(STATUS "Found stb_image.h at: ${STB_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Could not find stb_image.h")
endif()

message(STATUS "Looking for tiny_obj_loader.h")
find_path(TINYOBJLOADER_INCLUDE_DIR tiny_obj_loader.h)
if(TINYOBJLOADER_INCLUDE_DIR)
    message(STATUS "Found tiny_obj_loader.h at: ${TINYOBJLOADER_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Could not find tiny_obj_loader.h")
endif()

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