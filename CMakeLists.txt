cmake_minimum_required(VERSION 3.20)
project(Superbuild LANGUAGES CXX)

option(BUILD_CODE "Build libA" ON)
option(BUILD_PCL "Build libB" ON)
option(BUILD_GMAKE "Build libC" ON)

message(STATUS "this is a hotfix and should be changed later")
include_directories(${CMAKE_SOURCE_DIR}/lib/cpp)
message(STATUS "hotfix en is here for the lib libraries")

add_library(common_lib
    lib/cpp/GMAKE_EXCEPTION.cpp
)

target_include_directories(common_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/cpp
)

if(BUILD_GMAKE)
    add_subdirectory(GMake)
endif()

if(BUILD_CODE)
    add_subdirectory(the_code)
endif()

if(BUILD_PCL)
    add_subdirectory(PythonCompilerLibrary)
endif()