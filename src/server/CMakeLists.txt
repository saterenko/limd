cmake_minimum_required(VERSION 2.8)
project(limd_server)

FILE(GLOB SERVER_SOURCES *.cc)

add_library(limd_server_lib STATIC ${SERVER_SOURCES})

target_link_libraries(limd_server_lib pthread)
