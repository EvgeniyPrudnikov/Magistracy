cmake_minimum_required(VERSION 3.6)
project(MergeNews)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I/usr/include/jsoncpp -pthread -O2 \
  -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 \
  -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0 \
  -L/usr/local/lib -lmongocxx -lbsoncxx")

set(SOURCE_FILES MergeNews.cpp jsoncpp.cpp json/json.h json/json-forwards.h)


add_executable(MergeNews ${SOURCE_FILES})

SET(CMAKE_CXX_COMPILER g++)