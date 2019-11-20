# Function for building single example
function(buildExample EXAMPLE_NAME)
    SET(EXAMPLE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME})
    message(STATUS "Generating project file for example in ${EXAMPLE_FOLDER}")
    # example source files
    file(GLOB SOURCE ${EXAMPLE_FOLDER}/*.cpp)
    # third-party source files
    if(EXISTS ${EXAMPLE_NAME} STREQUAL "17.dear_imgui")
        file(GLOB ADD_SOURCE "../external/imgui/*.cpp")
        SET(SOURCE ${SOURCE} ${ADD_SOURCE})
    endif()
    # Add shaders
    SET(SHADER_DIR "../resources/shaders/${EXAMPLE_NAME}")
    file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.geom" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.comp")
    source_group("Shaders" FILES ${SHADERS})
    if(WIN32)
        add_executable(${EXAMPLE_NAME} WIN32 ${SOURCE} ${SHADERS})
        target_link_libraries(${EXAMPLE_NAME} common ${LIBS})
    else()

    endif()

    set_target_properties(${EXAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/win${BITS})
    set_target_properties(${EXAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/win${BITS})
    set_target_properties(${EXAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/win${BITS})
    set_target_properties(${EXAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/win${BITS})
endfunction(buildExample)

function(buildExamples)
    foreach(EXAMPLE ${EXAMPLES})
        buildExample(${EXAMPLE})
    endforeach(EXAMPLE)
endfunction(buildExamples)

# example list
set(EXAMPLES
    01.hello_window
    02.hello_window_color_clear
    03.triangle
    04.quadrangle_indices
    05.triangle_restart_index
    06.triangle_instancing
    07.quad_with_texture
    08.cube_with_camera
    09.transformations
    10.model_loading
    11.cull_face
    12.stencil_buffer
    13.blinn_phong_lighting
    14.blending
    15.render_to_texture
    16.normal_mapping
    17.dear_imgui
    18.uniform_buffer_object
    19.skybox_reflect
    20.parallax_mapping
    21.orthographic_shadow_mapping
    22.omnidirectional_shadow_mapping
    23.perspective_shadow_mapping
    24.cascaded_shadow_mapping
    25.variance_shadow_mapping
    26.bloom_with_hdr
    27.compute_shader_edgedetect
    28.compute_shader_particles
    29.basic_pbr
    30.image_based_lighting_pbr
    31.subsurface_scattering
)

buildExamples()