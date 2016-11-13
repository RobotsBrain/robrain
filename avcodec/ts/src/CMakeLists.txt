include_directories(
	${TOPDIR}/avcodec/ts/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(tscodec ${DIR_SRCS} ${HEADERFILES})
