cmake_minimum_required(VERSION 2.8)
project(limd_client)

FILE(GLOB CLIENT_SOURCES *.cc)

add_library(limd_client_lib STATIC ${CLIENT_SOURCES})

target_link_libraries(limd_client_lib pthread)
