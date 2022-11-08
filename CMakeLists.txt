cmake_minimum_required(VERSION 3.0)
project(cachesim)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    cache.cpp
    main.cpp)

add_executable(cachesim ${SOURCE_FILES})