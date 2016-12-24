cmake_minimum_required(VERSION 3.6)
project(lab6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES lab6.c)

add_executable(lab6 ${SOURCE_FILES})


find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpiCC)