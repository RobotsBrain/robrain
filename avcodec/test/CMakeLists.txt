set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/transmit/live555_rtsp/test)

include_directories(
	${TOPDIR}/basics/base/include
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs
)


#################################
add_executable(TestRtspServer TestRtspServer.cpp)
target_link_libraries(TestRtspServer live555_rtsp liveMedia UsageEnvironment BasicUsageEnvironment groupsock base pthread)

