set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/rtmp-client/test)

include_directories(
	${TOPDIR}/rbav/include
	${TOPDIR}/flv/include
	${TOPDIR}/rtmp/src
	${TOPDIR}/3rdparty/prebuild/include/
)

link_directories(
	${TOPDIR}/3rdparty/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestLiveRtmpStream TestLiveRtmpStream.cpp)
target_link_libraries(TestLiveRtmpStream rtmpclient rtmp flv ssl crypto z pthread)


#################################
add_executable(TestFetchRtmpStream TestFetchRtmpStream.cpp)
target_link_libraries(TestFetchRtmpStream rtmpclient rtmp flv ssl crypto z pthread)
