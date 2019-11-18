include_directories(
	${TOPDIR}/server/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(hlsserver ${DIR_SRCS} ${HEADERFILES})

