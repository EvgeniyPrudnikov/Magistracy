cmake_minimum_required(VERSION 3.6)
project(labDevNull)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(labDevNull ${SOURCE_FILES})