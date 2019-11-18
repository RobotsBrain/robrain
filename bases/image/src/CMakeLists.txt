include_directories(
	${TOPDIR}/image/include
	${TOPDIR}/prebuild/include/jpeg
	${TOPDIR}/prebuild/include/zbar
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(image ${DIR_SRCS} ${HEADERFILES})
