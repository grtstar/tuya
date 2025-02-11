cmake_minimum_required(VERSION 3.5)

# Abseil requires C++11
set(CMAKE_CXX_STANDARD 11)  


include_directories(.)
add_library(rksound ./rksound.cpp)	