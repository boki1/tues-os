cmake_minimum_required(VERSION 3.22)
project(net_examples)

add_executable(groupchat_bozho groupchat_bozho.c)
target_include_directories(groupchat_bozho PRIVATE .)
