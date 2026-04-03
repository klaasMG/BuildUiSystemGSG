cmake_minimum_required(VERSION 3.18)
project(GMake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# Header-only utility library
add_library(utils INTERFACE)



add_executable(GMake build_dependecy_graph.cpp
        SimpleASTGMAKE.h
        SimpleASTGMAKE.cpp
        TokeniserGMAKE.h
        TokeniserGMAKE.cpp
        Tokens.h
        file_utils.h
        file_utils.cpp
        GMakeTypes.h
        GMakeTypes.cpp)

target_link_libraries(GMake PRIVATE common_lib)

set_target_properties(GMake PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR})

if(MINGW)
    target_link_libraries(GMake PRIVATE -static-libgcc -static-libstdc++ -static)
endif()
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()