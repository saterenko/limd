cmake_minimum_required(VERSION 2.8)
project(limd_base)

FILE(GLOB BASE_SOURCES *.cc)

add_library(limd_base_lib STATIC ${BASE_SOURCES})

target_link_libraries(limd_base_lib pthread)
