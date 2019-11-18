set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/client/test)

include_directories(
	${TOPDIR}/client/include
	${TOPDIR}/client/src
)

link_directories(
)


#################################
add_executable(TestHttp TestHttp.cpp)
target_link_libraries(TestHttp hlsclient curl)

#################################
add_executable(TestM3U8FileParser TestM3U8FileParser.cpp)
target_link_libraries(TestM3U8FileParser hlsclient curl)


#################################
add_executable(TestM3u8Download TestM3u8Download.cpp)
target_link_libraries(TestM3u8Download hlsclient curl)


#################################
add_executable(TestStreamDownload TestStreamDownload.cpp)
target_link_libraries(TestStreamDownload hlsclient curl)