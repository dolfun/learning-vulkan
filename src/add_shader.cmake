function(add_shader TARGET SHADER)
    find_program(GLSLC glslc)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
    get_filename_component(SHADER_NAME ${SHADER} NAME_WLE)
    get_filename_component(SHADER_EXT ${SHADER} EXT)
    string(REPLACE "." "" SHADER_EXT ${SHADER_EXT})
    set(SHADER_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
    set(SHADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}_${SHADER_EXT}.spv)

    add_custom_command(
           OUTPUT ${SHADER_OUTPUT_PATH}
           COMMAND ${GLSLC} ${SHADER_SOURCE_PATH} -o ${SHADER_OUTPUT_PATH}
           DEPENDS ${SHADER}
           VERBATIM)

    set_source_files_properties(${SHADER_OUTPUT_PATH} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${SHADER_OUTPUT_PATH})
endfunction(add_shader)