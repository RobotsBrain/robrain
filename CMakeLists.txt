CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

set(TOPDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(OUTDIR ${TOPDIR}/out/)
set(BUILDDIR ${TOPDIR}/build-cmake/camera)

#include(${TOPDIR}/)
#include(${BOARDDIR}/)

#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

#add_compile_options("--std=c++11")
#add_definitions(-DBUILD_SSL)
#add_compile_options(-DBUILD_SSL)

add_definitions(-DBUILD_DUMP_H264FILE)

add_subdirectory(${TOPDIR}/base/src ${BUILDDIR}/base/src)
add_subdirectory(${TOPDIR}/base/test ${BUILDDIR}/base/test)

add_subdirectory(${TOPDIR}/platform/linux ${BUILDDIR}/platform/linux)
