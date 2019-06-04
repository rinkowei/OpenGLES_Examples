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

add_compile_options(-std=c++17)

find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

if(WIN32)
    set(LIBS glfw3 libGLESv2 libMaliEmulator)
else()
    set(LIBS )
endif(WIN32)

set(EXAMPLES
    1.hello_window
)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

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