add_executable(main)
target_sources(main PRIVATE
    main.cc
    application.h application.cc
    stb_image_implementation.cc
    debug_messenger.h
	utility.h
)
target_include_directories(main PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(main PRIVATE glfw glm::glm Vulkan::Vulkan)
target_compile_features(main PRIVATE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(main PRIVATE -static -Wall -Wextra -Wpedantic -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(main PRIVATE /MT /W4 /WX)
endif()

set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include(add_shader.cmake)
add_shader(main shaders/shader.vert)
add_shader(main shaders/shader.frag)