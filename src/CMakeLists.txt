# Function for building single example
function(buildExample EXAMPLE_NAME)
    SET(EXAMPLE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME})
    message(STATUS "Generating project file for example in ${EXAMPLE_FOLDER}")
    # example source files
    file(GLOB SOURCE ${EXAMPLE_FOLDER}/*.cpp)
    # third-party source files
    if(EXISTS ${EXAMPLE_NAME} STREQUAL "17.dear_imgui")
        file(GLOB ADD_SOURCE "../include/imgui/*.cpp")
        SET(SOURCE ${SOURCE} ${ADD_SOURCE})
    endif()
    # Add shaders
    SET(SHADER_DIR "../resources/shaders/${EXAMPLE_NAME}")
    file(GLOB SHADERS "${SHADER_DIR}/*.vs" "${SHADER_DIR}/*.fs")
    source_group("Shaders" FILES ${SHADERS})
    if(WIN32)
        add_executable(${EXAMPLE_NAME} ${SOURCE} ${SHADERS})
        target_link_libraries(${EXAMPLE_NAME} ${LIBS})
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
    03.hello_triangle
    04.hello_triangle_indices
    05.hello_triangle_restart_index
    06.hello_triangle_instancing
    07.quad_with_texture
    08.cube_with_camera
    09.transformations
    10.model_loading
    11.cull_face
    12.object_outlining
    13.blinn_phong_lighting
    14.blending
    15.offscreen_rendering
    16.normal_mapping
    17.dear_imgui
)

buildExamples()