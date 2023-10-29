cmake_minimum_required(VERSION 3.26)
project(smallheap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(smallheap smallheap.cpp)
target_include_directories(smallheap PRIVATE .)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(smallheap-test test.cpp)
target_link_libraries(smallheap-test PRIVATE smallheap GTest::gtest_main)

enable_testing()

include(GoogleTest)
gtest_discover_tests(smallheap-test)
