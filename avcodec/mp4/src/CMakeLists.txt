include_directories(
	${TOPDIR}/avcodec/mp4/include
	${TOPDIR}/opensource/prebuild/include/mp4v2/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(avcodec ${DIR_SRCS} ${HEADERFILES})
