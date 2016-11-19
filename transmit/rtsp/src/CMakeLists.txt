include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/transmit/live555_rtsp/include
	${TOPDIR}/opensource/prebuild/include/live555/liveMedia/
	${TOPDIR}/opensource/prebuild/include/live555/groupsock/
	${TOPDIR}/opensource/prebuild/include/live555/UsageEnvironment/
	${TOPDIR}/opensource/prebuild/include/live555/BasicUsageEnvironment
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(live555_rtsp ${DIR_SRCS} ${HEADERFILES})