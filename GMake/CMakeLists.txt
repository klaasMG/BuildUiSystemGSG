cmake_minimum_required(VERSION 3.18)
project(GMake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(GMake build_dependecy_graph.cpp
        SimpleASTGMAKE.h
        Tokens.h
        GMAKE_EXEPTION.h
        file_utils.h
        GMakeTypes.h)

set_target_properties(GMake PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR})

if(MINGW)
    target_link_libraries(GMake PRIVATE -static-libgcc -static-libstdc++ -static)
endif()