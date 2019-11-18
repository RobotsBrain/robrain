include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/3rdparty/prebuild/include/live555/liveMedia/
	${TOPDIR}/3rdparty/prebuild/include/live555/groupsock/
	${TOPDIR}/3rdparty/prebuild/include/live555/UsageEnvironment/
	${TOPDIR}/3rdparty/prebuild/include/live555/BasicUsageEnvironment
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(rtsp ${DIR_SRCS} ${HEADERFILES})
