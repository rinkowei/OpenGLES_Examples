cmake_minimum_required (VERSION 3.2)
cmake_policy(VERSION 3.2)

project(OpenGLES_Examples)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_definitions(-DES_EXAMPLE_RESOURCES_DIR=\"${CMAKE_SOURCE_DIR}/resources/\")

add_compile_options(-std=c++17)

find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

if(WIN32)
    set(LIBS glfw3 libGLESv2 libMaliEmulator assimp)
else()
    set(LIBS )
endif(WIN32)

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
)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

add_library(Common "src/Common.cpp")
set(LIBS ${LIBS} Common)

foreach(EXAMPLE ${EXAMPLES})
    file(GLOB SOURCE
        "src/${EXAMPLE}/*.h"
        "src/${EXAMPLE}/*.cpp"
    )
    set(NAME "${EXAMPLE}")
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    if(WIN32)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${EXAMPLE}")
    endif(WIN32)
endforeach(EXAMPLE)