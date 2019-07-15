cmake_minimum_required (VERSION 3.4 FATAL_ERROR)
cmake_policy(VERSION 3.4)

set(NAME OpenGLES_Examples)
project(${NAME})

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/external)

math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")

if(WIN32 AND NOT MINGW)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
  
    link_directories(${CMAKE_SOURCE_DIR}/lib/win${BITS})
    set(LIBS glfw3 libGLESv2 assimp-vc140-mt)

    # copy DLLs to runtime_output_directory in advance
    file(GLOB DLLs ${CMAKE_SOURCE_DIR}/dll/win${BITS}/*.dll)
    set(DLL_DESTINATIONS
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/win${BITS} 
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/win${BITS}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/win${BITS}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/win${BITS} 
    )
    foreach(DLL_DESTINATION ${DLL_DESTINATIONS})
        file(MAKE_DIRECTORY ${DLL_DESTINATION})
        foreach(DLL ${DLLs})
            execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${DLL} ${DLL_DESTINATION}/)
        endforeach(DLL)
    endforeach(DLL_DESTINATION)
else()
    set(LIBS )
endif()

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DES_EXAMPLE_RESOURCES_DIR=\"${CMAKE_SOURCE_DIR}/resources/\")

if(MSVC_VERSION GREATER_EQUAL "1900")
    add_compile_options("/std:c++17")
else()
    add_compile_options("/std:c++14")
endif()

add_library(glad "external/glad/glad.c")
set(LIBS ${LIBS} glad)

add_subdirectory(common)
add_subdirectory(src)