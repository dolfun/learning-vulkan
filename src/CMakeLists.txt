add_executable(main)
target_sources(main PRIVATE
    main.cc
    application.h application.cc
    debug_messenger.h
	utility.h
)
target_link_libraries(main PRIVATE glfw Vulkan::Vulkan)
target_compile_features(main PRIVATE cxx_std_17)

if(MSVC)
	target_compile_options(main PRIVATE /MT /W4 /WX)
else()
	target_compile_options(main PRIVATE -static -Wall -Wextra -Wpedantic -Werror)
endif()

set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include(add_shader.cmake)
add_shader(main shaders/shader.vert)
add_shader(main shaders/shader.frag)