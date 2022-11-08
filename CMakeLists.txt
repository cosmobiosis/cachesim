cmake_minimum_required(VERSION 3.10)

project(cachesim VERSION 1.0)

add_executable(cachesim main.cpp)
add_library(cachesimlib cache.cpp)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
