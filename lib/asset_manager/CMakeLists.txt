cmake_minimum_required(VERSION 3.18)
project(asset_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# Find Python (for headers and libs)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Path to pybind11 headers in your venv
set(PYBIND11_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/pybind11/include")

# Include directories
include_directories(${Python3_INCLUDE_DIRS} ${PYBIND11_INCLUDE_DIR})

add_library(asset_manager asset_manager.cpp asset_manager.h)

target_link_libraries(asset_manager PRIVATE common_lib)

target_include_directories(asset_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})