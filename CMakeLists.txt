cmake_minimum_required(VERSION 3.20)
project(Superbuild LANGUAGES CXX)

function(PybindIntegration)



endfunction()

option(BUILD_CODE "Build libA" ON)
option(BUILD_PCL "Build libB" ON)
option(BUILD_GMAKE "Build libC" ON)
option(BUILD_ASSET_MANAGER "Build liD" ON)

message(STATUS "this is a hotfix and should be changed later")
include_directories(${CMAKE_SOURCE_DIR}/lib/cpp)
message(STATUS "hotfix en is here for the lib libraries")

add_library(common_lib
        lib/cpp_base/GMAKE_EXCEPTION.cpp
        lib/cpp_base/string_utils.h
        lib/cpp_base/string_utils.cpp
        lib/cpp_base/Error.cpp
        lib/cpp_base/Error.h
)

target_include_directories(common_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/cpp_base
)

if (BUILD_ASSET_MANAGER)
    add_subdirectory(${CMAKE_SOURCE_DIR}/lib/asset_manager)
endif ()

if(BUILD_GMAKE)
    add_subdirectory(GMake)
endif()

if(BUILD_CODE)
    add_subdirectory(the_code)
endif()

if(BUILD_PCL)
    add_subdirectory(PythonCompilerLibrary)
endif()