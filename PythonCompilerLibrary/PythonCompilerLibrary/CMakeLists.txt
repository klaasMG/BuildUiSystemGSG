cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)

project(PCL)

add_executable(PCL MainPCL.cpp
        Tokeniser.h
        tokens.h
        AST_old.h
        AST.h
        AST.cpp)