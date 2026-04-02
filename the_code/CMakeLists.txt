cmake_minimum_required(VERSION 3.18)
project(event_struct LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# Find Python (for headers and libs)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Path to pybind11 headers in your venv
set(PYBIND11_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/pybind11/include")

# Include directories
include_directories(${Python3_INCLUDE_DIRS} ${PYBIND11_INCLUDE_DIR} the_code )

# Build as a Python extension module (.pyd)
add_library(event_struct SHARED
        event_struct.cpp)

target_link_libraries(event_struct PRIVATE common_lib)

# Link with Python library
target_link_libraries(event_struct PRIVATE ${Python3_LIBRARIES})

# Output as .pyd (no "lib" prefix)
set_target_properties(event_struct PROPERTIES PREFIX "" SUFFIX ".pyd")

add_library(GridPacker SHARED
        GridPacker.cpp)

target_link_libraries(GridPacker PRIVATE common_lib)

target_link_libraries(GridPacker PRIVATE ${Python3_LIBRARIES})

set_target_properties(GridPacker PROPERTIES PREFIX "" SUFFIX ".pyd")

set_target_properties(GridPacker PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/the_ui_tree_build)

add_library(font_holder SHARED font_loader.cpp
        stb_treutype.h
)

target_link_libraries(font_holder PRIVATE common_lib)

target_link_libraries(font_holder PRIVATE
        ${Python3_LIBRARIES}
)

set_target_properties(font_holder PROPERTIES
        PREFIX ""
        SUFFIX ".pyd"
)

set_target_properties(font_holder PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/the_ui_tree_build)

add_library(shader_preprocessing SHARED shader_preprocesing.cpp)

target_link_libraries(shader_preprocessing PRIVATE common_lib)

target_link_libraries(shader_preprocessing PRIVATE
        ${Python3_LIBRARIES}
)

set_target_properties(shader_preprocessing PROPERTIES
        PREFIX ""
        SUFFIX ".pyd"
)