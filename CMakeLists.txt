cmake_minimum_required(VERSION 3.20)
project(Superbuild LANGUAGES CXX)

option(BUILD_CODE "Build libA" ON)
option(BUILD_PCL "Build libB" ON)
option(BUILD_GMAKE "Build libC" ON)

if(BUILD_GMAKE)
    add_subdirectory(GMake)
endif()

if(BUILD_CODE)
    add_subdirectory(the_code)
endif()

if(BUILD_PCL)
    add_subdirectory(PythonCompilerLibrary)
endif()

#add_executable(f_help the_ui_tree_build/assets/shaders/fgg.cpp)
#target_link_options(f_help PRIVATE -static -static-libgcc -static-libstdc++)