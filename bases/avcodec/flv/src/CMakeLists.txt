include_directories(
	${TOPDIR}/avcodec/flv/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(flvcodec ${DIR_SRCS} ${HEADERFILES})
