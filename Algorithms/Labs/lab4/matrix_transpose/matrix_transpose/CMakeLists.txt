cmake_minimum_required(VERSION 3.6)
project(matrix_transpose)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    matrix_transpose.cpp)

add_executable(matrix_transpose ${SOURCE_FILES})