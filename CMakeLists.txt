cmake_minimum_required(VERSION 3.18)
project(event_struct LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# Find Python (for headers and libs)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Path to pybind11 headers in your venv
set(PYBIND11_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/pybind11/include/pybind")

# Include directories
include_directories(${Python3_INCLUDE_DIRS} ${PYBIND11_INCLUDE_DIR} the_code )

# Build as a Python extension module (.pyd)
add_library(event_struct SHARED the_code/event_struct.cpp)

# Link with Python library
target_link_libraries(event_struct PRIVATE ${Python3_LIBRARIES})

# Output as .pyd (no "lib" prefix)
set_target_properties(event_struct PROPERTIES PREFIX "" SUFFIX ".pyd")

add_library(font_holder SHARED
        the_code/font_loader.cpp
        the_code/stb_treutype.h
)

target_link_libraries(font_holder PRIVATE
        ${Python3_LIBRARIES}
)

set_target_properties(font_holder PROPERTIES
        PREFIX ""
        SUFFIX ".pyd"
)

add_library(shader_preprocessing SHARED the_code/shader_preprocesing.cpp
        the_code/TokeniserGMAKE.h)

target_link_libraries(shader_preprocessing PRIVATE
        ${Python3_LIBRARIES}
)

set_target_properties(shader_preprocessing PROPERTIES
        PREFIX ""
        SUFFIX ".pyd"
)

add_executable(shader_include_graph the_code/build_dependecy_graph.cpp
        the_code/SimpleASTGMAKE.h
        the_code/Tokens.h)

set_target_properties(shader_include_graph PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MINGW)
    target_link_libraries(shader_include_graph -static-libgcc -static-libstdc++ -static)
    # Or for the whole project:
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()