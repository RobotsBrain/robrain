CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

set(TOPDIR ${CMAKE_CURRENT_SOURCE_DIR})

include(${TOPDIR}/cmake/${TOOLCHAIN}.cmake)

set(OUTDIR ${TOPDIR}/output/target)
set(BUILDDIR ${TOPDIR}/output/build/)


#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

#add_compile_options("--std=c++11")
#add_definitions(-DBUILD_SSL)
#add_compile_options(-DBUILD_SSL)

add_definitions(-DBUILD_DUMP_H264FILE)

add_subdirectory(${TOPDIR}/bases/base/src ${BUILDDIR}/bases/base/src)
add_subdirectory(${TOPDIR}/bases/image/src ${BUILDDIR}/bases/image/src)
#add_subdirectory(${TOPDIR}/network/src ${BUILDDIR}/network/src)
#add_subdirectory(${TOPDIR}/avcodec/mp4/src ${BUILDDIR}/avcodec/mp4/src)
#add_subdirectory(${TOPDIR}/avcodec/flv/src ${BUILDDIR}/avcodec/flv/src)
#add_subdirectory(${TOPDIR}/avcodec/ts/src ${BUILDDIR}/avcodec/ts/src)
#add_subdirectory(${TOPDIR}/avcodec/aac/src ${BUILDDIR}/avcodec/aac/src)
#add_subdirectory(${TOPDIR}/stream_media/rtsp/src ${BUILDDIR}/stream_media/rtsp/src)
#add_subdirectory(${TOPDIR}/stream_media/rtmp/src ${BUILDDIR}/stream_media/rtmp/src)
#add_subdirectory(${TOPDIR}/transmit/rtsp/src ${BUILDDIR}/transmit/rtsp/src)
#add_subdirectory(${TOPDIR}/transmit/src ${BUILDDIR}/transmit/src)
add_subdirectory(${TOPDIR}/platform/linux/src ${BUILDDIR}/platform/linux/src)
add_subdirectory(${TOPDIR}/platform/linux/eye ${BUILDDIR}/platform/linux/eye)


#add_subdirectory(${TOPDIR}/basics/base/test ${BUILDDIR}/basics/base/test)
#add_subdirectory(${TOPDIR}/network/test ${BUILDDIR}/network/test)
#add_subdirectory(${TOPDIR}/avcodec/mp4/test ${BUILDDIR}/avcodec/mp4/test)
#add_subdirectory(${TOPDIR}/avcodec/flv/test ${BUILDDIR}/avcodec/flv/test)
#add_subdirectory(${TOPDIR}/avcodec/ts/test ${BUILDDIR}/avcodec/ts/test)
#add_subdirectory(${TOPDIR}/avcodec/aac/test ${BUILDDIR}/avcodec/aac/test)
#add_subdirectory(${TOPDIR}/stream_media/rtsp/test ${BUILDDIR}/stream_media/rtsp/test)
#add_subdirectory(${TOPDIR}/stream_media/rtmp/test ${BUILDDIR}/stream_media/rtmp/test)
#add_subdirectory(${TOPDIR}/transmit/rtsp/test ${BUILDDIR}/transmit/rtsp/test)