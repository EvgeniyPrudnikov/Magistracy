cmake_minimum_required(VERSION 3.6)
project(list_ranking)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    list_ranking.cpp)

add_executable(list_ranking ${SOURCE_FILES})