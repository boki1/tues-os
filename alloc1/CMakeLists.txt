cmake_minimum_required(VERSION 3.26)
project(alloc1)

add_library(alloc1 alloc1.c)
target_include_directories(alloc1 PUBLIC ./)

add_executable(example main.c)
target_link_libraries(example PUBLIC alloc1)
target_include_directories(example PUBLIC ./)
