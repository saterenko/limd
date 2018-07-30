cmake_minimum_required(VERSION 2.8)
project(limd)

set(SERVER_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/limd.cc)

FILE(GLOB SERVER_SOURCES *.cc)
LIST(REMOVE_ITEM SERVER_SOURCES ${SERVER_MAIN_SOURCES})

add_library(limd_server_lib STATIC ${SERVER_SOURCES})
add_executable(${PROJECT_NAME} ${SERVER_MAIN_SOURCES})

target_link_libraries(${PROJECT_NAME}
limd_base_lib
limd_server_lib
xxhash_lib
ev
pthread)
