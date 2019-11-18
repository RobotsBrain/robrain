include_directories(
	${TOPDIR}/client/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(hlsclient ${DIR_SRCS} ${HEADERFILES})

