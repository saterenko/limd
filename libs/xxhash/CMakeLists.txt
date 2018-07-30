cmake_minimum_required(VERSION 2.8)
project(xxhash)

add_library(xxhash_lib STATIC xxhash.c)

target_link_libraries(xxhash_lib)
