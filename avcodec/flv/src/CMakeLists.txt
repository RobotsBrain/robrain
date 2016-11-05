include_directories(
	${TOPDIR}/avcodec/flv/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(flv ${DIR_SRCS} ${HEADERFILES})
