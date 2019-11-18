set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/platform/linux/eye)

include_directories(
    ${TOPDIR}/bases/base/include
    ${TOPDIR}/bases/image/include
    ${TOPDIR}/platform/linux/src
    ${TOPDIR}/3rdparty/prebuild/include/jrtplib
    ${TOPDIR}/3rdparty/prebuild/include/x264
    ${TOPDIR}/3rdparty/prebuild/include/jpeg
)

link_directories(
    ${TOPDIR}/3rdparty/prebuild/libs/ubuntu
)


#################################
aux_source_directory(. DIR_SRCS)
add_executable(eye ${DIR_SRCS})

target_link_libraries(eye device image jpeg base x264 asound pthread)