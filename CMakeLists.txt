cmake_minimum_required(VERSION 3.20)
project(Superbuild LANGUAGES CXX)

option(BUILD_CODE "Build libA" ON)
option(BUILD_PCL "Build libB" ON)

if(BUILD_CODE)
    add_subdirectory(the_code)
endif()

if(BUILD_PCL)
    add_subdirectory(PythonCompilerLibrary)
endif()