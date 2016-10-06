set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/platform/linux)

include_directories(
    ${TOPDIR}/base/include
    ${TOPDIR}/transmit/include
    ${TOPDIR}/opensource/prebuild/include/jrtplib
    ${TOPDIR}/opensource/prebuild/include/x264
)

link_directories(
    ${TOPDIR}/opensource/prebuild/libs
)


#################################
aux_source_directory(. DIR_SRCS)
add_executable(robrain ${DIR_SRCS})

target_link_libraries(robrain transmit base x264 asound jrtp pthread)