cmake_minimum_required(VERSION 3.6)
project(NewsService)


set(CMAKE_CXX_FLAGS " --std=c++11 ${SOURCE_FILES} -shared -o libNewsService.so -lfastcgi-daemon2 -fPIC -O2   \
-I/usr/local/include/mongocxx/v_noabi \
-I/usr/local/include/libmongoc-1.0 \
-I/usr/local/include/bsoncxx/v_noabi \
-I/usr/local/include/libbson-1.0  \
-L/usr/local/lib -lmongocxx -lbsoncxx")

set(SOURCE_FILES NewsService.cpp NewsServiceHandler.cpp NSMongoStorage.cpp)

add_library(NewsService SHARED ${SOURCE_FILES})

SET(CMAKE_CXX_COMPILER c++)