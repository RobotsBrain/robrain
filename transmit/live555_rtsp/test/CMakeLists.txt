set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/transmit/live555_rtsp/test)

include_directories(
	${TOPDIR}/basics/base/include
    ${TOPDIR}/opensource/prebuild/include/live555/liveMedia/
	${TOPDIR}/opensource/prebuild/include/live555/groupsock/
	${TOPDIR}/opensource/prebuild/include/live555/UsageEnvironment/
	${TOPDIR}/opensource/prebuild/include/live555/BasicUsageEnvironment/
	${TOPDIR}/transmit/live555_rtsp/src
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs
)


#################################
add_executable(TestRtspServer TestRtspServer.cpp)
target_link_libraries(TestRtspServer live555_rtsp liveMedia UsageEnvironment BasicUsageEnvironment groupsock base pthread)


#################################
add_executable(TestG711RtspServer TestG711RtspServer.cpp)
target_link_libraries(TestG711RtspServer live555_rtsp liveMedia BasicUsageEnvironment UsageEnvironment groupsock base pthread)


#################################
add_executable(TestLiveStreamRtspServer TestLiveStreamRtspServer.cpp)
target_link_libraries(TestLiveStreamRtspServer live555_rtsp liveMedia BasicUsageEnvironment UsageEnvironment groupsock base pthread)