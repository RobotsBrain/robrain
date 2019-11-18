set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/rtsp/server/test)

include_directories(
	${TOPDIR}/basics/base/include
    ${TOPDIR}/3rdparty/prebuild/include/live555/liveMedia/
	${TOPDIR}/3rdparty/prebuild/include/live555/groupsock/
	${TOPDIR}/3rdparty/prebuild/include/live555/UsageEnvironment/
	${TOPDIR}/3rdparty/prebuild/include/live555/BasicUsageEnvironment/
	${TOPDIR}/rtsp/server/src
)

link_directories(
	${TOPDIR}/3rdparty/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestRtspServer TestRtspServer.cpp)
target_link_libraries(TestRtspServer rtsp liveMedia UsageEnvironment BasicUsageEnvironment groupsock base pthread)


#################################
add_executable(TestG711RtspServer TestG711RtspServer.cpp)
target_link_libraries(TestG711RtspServer rtsp liveMedia BasicUsageEnvironment UsageEnvironment groupsock base pthread)


#################################
add_executable(TestLiveStreamRtspServer TestLiveStreamRtspServer.cpp)
target_link_libraries(TestLiveStreamRtspServer rtsp liveMedia BasicUsageEnvironment UsageEnvironment groupsock base pthread)
