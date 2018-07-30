cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -Wall -Werror -fpermissive")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-fno-deduce-init-list -msse4.2 -fPIC -m64 -fno-omit-frame-pointer)
    add_definitions(-MP -MMD -MF -MQ -O3)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_ON -ggdb -fstack-protector-all)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build.${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build.${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build.${CMAKE_BUILD_TYPE}")

include_directories(libs libs/libev src /usr/local/include)
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} /usr/lib /usr/local/lib)

set(INSTALL_PREFIX "/usr/local/limd")
set(INSTALL_BIN "/usr/local/limd")
set(INSTALL_CONF "/usr/local/limd")

add_subdirectory(libs)
add_subdirectory(src)

install(CODE "EXECUTE_PROCESS(COMMAND chown -R limd:limd ${INSTALL_PREFIX})")
