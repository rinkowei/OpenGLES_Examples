cmake_minimum_required (VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

set(NAME OpenGLES_Examples)
project(${NAME})

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include_directories(${CMAKE_SOURCE_DIR}/include)

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DES_EXAMPLE_RESOURCES_DIR=\"${CMAKE_SOURCE_DIR}/resources/\")

if(MSVC_VERSION GREATER_EQUAL "1900")
    add_compile_options("/std:c++17")
else()
    add_compile_options("/std:c++14")
endif()

math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")

if(WIN32 AND NOT MINGW)
    link_directories(${CMAKE_SOURCE_DIR}/lib/win${BITS})
    set(LIBS glfw3 libGLESv2 assimp-vc140-mt)
else()
    set(LIBS )
endif(WIN32 AND NOT MINGW)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

add_library(Common "src/Common.cpp")
set(LIBS ${LIBS} Common)

# Build all examples
function(buildExamples)
	foreach(EXAMPLE ${EXAMPLES})
		buildExample(${EXAMPLE})
	endforeach(EXAMPLE)
endfunction(buildExamples)

add_subdirectory(src)