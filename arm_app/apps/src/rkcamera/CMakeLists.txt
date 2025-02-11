cmake_minimum_required(VERSION 3.5)

# Abseil requires C++11
set(CMAKE_CXX_STANDARD 11)  


include_directories(.)
include_directories($ENV{SYSROOT}/usr/include/camera_engine_rkisp/interface)
include_directories($ENV{SYSROOT}/usr/include/rockchip)
include_directories(./mpp/inc)
include_directories(./osal/inc)
include_directories(./utils)

add_library(rkcamera ./rkcamera.cpp
	./encoder.cpp
)	