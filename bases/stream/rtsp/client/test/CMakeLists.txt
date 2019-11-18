set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/rtsp/client/test)

include_directories(
	${TOPDIR}/basics/base/include
    ${TOPDIR}/3rdparty/prebuild/include/live555/liveMedia/
	${TOPDIR}/3rdparty/prebuild/include/live555/groupsock/
	${TOPDIR}/3rdparty/prebuild/include/live555/UsageEnvironment/
	${TOPDIR}/3rdparty/prebuild/include/live555/BasicUsageEnvironment/
	${TOPDIR}/rtsp/client/src
)

link_directories(
	${TOPDIR}/3rdparty/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestRtspServer TestRtspServer.cpp)
target_link_libraries(TestRtspServer rtsp liveMedia UsageEnvironment BasicUsageEnvironment groupsock base pthread)

