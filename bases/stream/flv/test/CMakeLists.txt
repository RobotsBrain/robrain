set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/flv/test)

include_directories(
	${TOPDIR}/rbav/include
	${TOPDIR}/flv/include
)

link_directories(
)


#################################
add_executable(TestFlvDemux TestFlvDemux.cpp)
target_link_libraries(TestFlvDemux flv)


#################################
add_executable(TestFlvMux TestFlvMux.cpp)
target_link_libraries(TestFlvMux flv)