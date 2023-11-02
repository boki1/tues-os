cmake_minimum_required(VERSION 3.26)
project(04_O_allocator)

set(CMAKE_CXX_STANDARD 17)

add_executable(04_O_allocator main.cpp
        OAllocator.cpp
        OAllocator.h
        small_allocator.cpp
        sallocator.h)
