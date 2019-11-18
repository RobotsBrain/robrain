CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

set(TOPDIR ${CMAKE_CURRENT_SOURCE_DIR})

include(${TOPDIR}/cmake/${TOOLCHAIN}.cmake)

set(OUTDIR ${TOPDIR}/out/)
set(BUILDDIR ${TOPDIR}/build/)


#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

#add_compile_options("--std=c++11")
#add_definitions(-DBUILD_SSL)
#add_compile_options(-DBUILD_SSL)

add_subdirectory(${TOPDIR}/basics/base/src ${BUILDDIR}/basics/base/src)
add_subdirectory(${TOPDIR}/basics/base/test ${BUILDDIR}/basics/base/test)

add_subdirectory(${TOPDIR}/flv/src ${BUILDDIR}/flv/src)
add_subdirectory(${TOPDIR}/flv/test ${BUILDDIR}/flv/test)

add_subdirectory(${TOPDIR}/rbav/src ${BUILDDIR}/rbav/src)
add_subdirectory(${TOPDIR}/rbav/test ${BUILDDIR}/rbav/test)

add_subdirectory(${TOPDIR}/rtmp/src ${BUILDDIR}/rtmp/src)
add_subdirectory(${TOPDIR}/rtmp/test ${BUILDDIR}/rtmp/test)

#add_subdirectory(${TOPDIR}/rtp/src ${BUILDDIR}/rtp/src)
#add_subdirectory(${TOPDIR}/rtp/test ${BUILDDIR}/rtp/test)

add_subdirectory(${TOPDIR}/rtsp/server/src ${BUILDDIR}/rtsp/server/src)
add_subdirectory(${TOPDIR}/rtsp/server/test ${BUILDDIR}/rtsp/server/test)

add_subdirectory(${TOPDIR}/rtsp/client/src ${BUILDDIR}/rtsp/client/src)
#add_subdirectory(${TOPDIR}/hls/src ${BUILDDIR}/hls/src)
#add_subdirectory(${TOPDIR}/hls/test ${BUILDDIR}/hls/test)
