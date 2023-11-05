cmake_minimum_required(VERSION 3.24)
project(ualloc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ualloc ualloc.cpp)
target_include_directories(ualloc PRIVATE .)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(ualloc-test test.cpp)
target_link_libraries(ualloc-test PRIVATE ualloc GTest::gtest_main)

enable_testing()

include(GoogleTest)
gtest_discover_tests(ualloc-test)

