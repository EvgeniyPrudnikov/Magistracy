cmake_minimum_required(VERSION 3.6)
project(NewsService)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -lfastcgi-daemon2 -std=c++11")

set(SOURCE_FILES NewsService.cpp)

add_library(NewsService SHARED ${SOURCE_FILES})