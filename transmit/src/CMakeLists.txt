include_directories(
	${TOPDIR}/transmit/include
	${TOPDIR}/opensource/prebuild/include/jrtplib
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(transmit ${DIR_SRCS} ${HEADERFILES})
