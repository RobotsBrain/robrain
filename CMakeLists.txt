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

add_definitions(-DBUILD_DUMP_H264FILE)

add_subdirectory(${TOPDIR}/basics/base/src ${BUILDDIR}/basics/base/src)
add_subdirectory(${TOPDIR}/basics/base/test ${BUILDDIR}/basics/base/test)

add_subdirectory(${TOPDIR}/image/src ${BUILDDIR}/image/src)

#add_subdirectory(${TOPDIR}/network/src ${BUILDDIR}/network/src)
#add_subdirectory(${TOPDIR}/network/test ${BUILDDIR}/network/test)

add_subdirectory(${TOPDIR}/avcodec/mp4/src ${BUILDDIR}/avcodec/mp4/src)
add_subdirectory(${TOPDIR}/avcodec/mp4/test ${BUILDDIR}/avcodec/mp4/test)

add_subdirectory(${TOPDIR}/avcodec/flv/src ${BUILDDIR}/avcodec/flv/src)
add_subdirectory(${TOPDIR}/avcodec/flv/test ${BUILDDIR}/avcodec/flv/test)

add_subdirectory(${TOPDIR}/transmit/live555_rtsp/src ${BUILDDIR}/transmit/live555_rtsp/src)
add_subdirectory(${TOPDIR}/transmit/live555_rtsp/test ${BUILDDIR}/transmit/live555_rtsp/test)

add_subdirectory(${TOPDIR}/transmit/rtmp/src ${BUILDDIR}/transmit/rtmp/src)
add_subdirectory(${TOPDIR}/transmit/rtmp/test ${BUILDDIR}/transmit/rtmp/test)

#add_subdirectory(${TOPDIR}/transmit/rtsp/src ${BUILDDIR}/transmit/rtsp/src)
#add_subdirectory(${TOPDIR}/transmit/rtsp/test ${BUILDDIR}/transmit/rtsp/test)

#add_subdirectory(${TOPDIR}/transmit/src ${BUILDDIR}/transmit/src)

#add_subdirectory(${TOPDIR}/platform/linux ${BUILDDIR}/platform/linux)
