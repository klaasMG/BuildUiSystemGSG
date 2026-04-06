cmake_minimum_required(VERSION 3.18)
project(asset_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_library(asset_manager asset_manager.cpp asset_manager.h)

target_link_libraries(asset_manager PRIVATE common_lib)

target_include_directories(asset_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})