cmake_minimum_required(VERSION 3.6)
project(lab1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS -pthread)

set(SOURCE_FILES main.c)
add_executable(lab1 ${SOURCE_FILES})