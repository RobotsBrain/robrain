set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/server/test)

include_directories(
	${TOPDIR}/server/include
	${TOPDIR}/server/src
)

link_directories(
)


#################################
add_executable(TestHlsServer TestHlsServer.cpp)
target_link_libraries(TestHlsServer hlsserver event)


#################################
#add_executable(TestHlsServer http_server.c response.c)
#target_link_libraries(TestHlsServer event)
