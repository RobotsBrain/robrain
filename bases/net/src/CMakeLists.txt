include_directories(
	${TOPDIR}/network/include
    ${TOPDIR}/base/include
    ${TOPDIR}/opensource/prebuild/include/pcap
    ${TOPDIR}/opensource/prebuild/include/adns
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(network ${DIR_SRCS} ${HEADERFILES})