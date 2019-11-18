set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/platform/linux)

include_directories(
    ${TOPDIR}/bases/base/include
    ${TOPDIR}/bases/image/include
    ${TOPDIR}/3rdparty/prebuild/include/jrtplib
    ${TOPDIR}/3rdparty/prebuild/include/x264
    ${TOPDIR}/3rdparty/prebuild/include/jpeg
)

include_directories(../include)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(device ${DIR_SRCS} ${HEADERFILES})
