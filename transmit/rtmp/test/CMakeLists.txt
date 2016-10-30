set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/transmit/rtmp/test)

include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/transmit/rtmp/src
	${TOPDIR}/opensource/prebuild/include/rtmpdump/
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs
)


#################################
add_executable(TestRtmpSendH264 TestRtmpSendH264.cpp)
target_link_libraries(TestRtmpSendH264 rtmpsend rtmp ssl crypt base pthread)


#################################
add_executable(TestRtmpRecv TestRtmpRecv.cpp)
target_link_libraries(TestRtmpRecv rtmpsend rtmp ssl crypt base pthread)