cmake_minimum_required(VERSION 3.24.0)
project(learning-vulkan VERSION 1.0)

include(CheckIncludeFileCXX)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

find_package(glm)
if (NOT glm_FOUND)
    CHECK_INCLUDE_FILE_CXX(glm/glm.hpp glm_hpp_FOUND)
    if (NOT glm_hpp_FOUND)
        message(FATAL_ERROR "Cannot find glm/glm.hpp")
    endif()
endif()

find_package(Stb)
if (NOT Stb_FOUND)
    CHECK_INCLUDE_FILE_CXX(stb_image.h stb_image_h_FOUND)
    if (NOT stb_image_h_FOUND)
        message(FATAL_ERROR "Cannot find stb_image.h")
    endif()
endif()

find_package(tinyobjloader CONFIG)
if (NOT tinyobjloader_FOUND)
    CHECK_INCLUDE_FILE_CXX(tiny_obj_loader.h tiny_obj_loader_h_FOUND)
    if (NOT tiny_obj_loader_h_FOUND)
        message(FATAL_ERROR "Cannot find tiny_obj_loader.h")
    endif()
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