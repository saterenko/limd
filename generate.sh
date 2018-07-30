#!/bin/sh

CMAKE_BUILD_TYPE=Debug

if [ $# -ge 1 ]; then
    if [ $1 != "Release" ] && [ $1 != "Debug" ]; then
        echo "Invalid build type: '"$1"'. Use 'Debug' or 'Release'." 1>&2
        exit 1
    fi
    CMAKE_BUILD_TYPE=$1
fi

BUILD_DIR=build.${CMAKE_BUILD_TYPE}

mkdir -p ${BUILD_DIR}
rm -f ${BUILD_DIR}/CMakeCache.txt

cmake -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} .
CMAKE_RETURN=$?

exit $CMAKE_RETURN
