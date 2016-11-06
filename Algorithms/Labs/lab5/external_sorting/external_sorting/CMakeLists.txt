cmake_minimum_required(VERSION 3.6)
project(external_sorting)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    external_sorting.cpp)

add_executable(external_sorting ${SOURCE_FILES})