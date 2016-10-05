set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/platform/linux)

include_directories(
    ${TOPDIR}/base/include
    ${TOPDIR}/opensource/prebuild/include/pcap
)

link_directories(
    ${TOPDIR}/opensource/prebuild/libs
)


#################################
aux_source_directory(. DIR_SRCS)
add_executable(testpcap ${DIR_SRCS})

target_link_libraries(testpcap pcap dbus-1)