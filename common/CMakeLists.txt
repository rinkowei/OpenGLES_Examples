file(GLOB COMMON_HEADERS "*.h")
file(GLOB COMMON_SRC "*.cpp" "../external/imgui/*.cpp")

if(WIN32 AND NOT MINGW)
    add_library(common STATIC ${COMMON_SRC})
    target_link_libraries(common ${LIBS} ${WINLIBS})
endif()