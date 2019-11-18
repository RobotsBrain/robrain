include_directories(
	${TOPDIR}/avcodec/aac/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(aaccodec ${DIR_SRCS} ${HEADERFILES})
