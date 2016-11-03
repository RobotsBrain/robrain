set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/transmit/rtmp/test)

include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/transmit/rtmp/src
	${TOPDIR}/opensource/prebuild/include/rtmpdump/
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestRtmpSendH264 TestRtmpSendH264.cpp)
target_link_libraries(TestRtmpSendH264 rtmpsend rtmp ssl crypto z base pthread)


#################################
add_executable(TestRtmpRecv TestRtmpRecv.cpp)
target_link_libraries(TestRtmpRecv rtmpsend rtmp ssl crypto z base pthread)


#################################
add_executable(TestRtmpSendFlv TestRtmpSendFlv.cpp)
target_link_libraries(TestRtmpSendFlv rtmpsend rtmp ssl crypto z base pthread)