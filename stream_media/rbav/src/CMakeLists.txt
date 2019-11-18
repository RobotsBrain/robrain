include_directories(
	${TOPDIR}/rbav/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(rbav ${DIR_SRCS} ${HEADERFILES})

