cmake_minimum_required(VERSION 3.26)
project(small-heap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(small-heap small_heap.cpp)
target_include_directories(small-heap PRIVATE .)

add_executable(small-heap-test test_fun.cpp)
target_link_libraries(small-heap-test small-heap)
target_include_directories(small-heap-test PRIVATE .)
