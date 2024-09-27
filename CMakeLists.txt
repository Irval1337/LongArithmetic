cmake_minimum_required(VERSION 3.28)
project(LongArithmetic)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(LongArithmetic
        Example.cpp
        Large.h
        Large.cpp)
