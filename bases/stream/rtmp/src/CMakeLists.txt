include_directories(
	${TOPDIR}/3rdparty/prebuild/include/
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(rtmpclient ${DIR_SRCS} ${HEADERFILES})