cmake_minimum_required(VERSION 3.22)
project(fast_copy)

add_executable(fastcp fastcp.cpp copy.cpp)
target_include_directories(fastcp PRIVATE .)

add_executable(rename rename.cpp copy.cpp)
target_include_directories(rename PRIVATE .)
