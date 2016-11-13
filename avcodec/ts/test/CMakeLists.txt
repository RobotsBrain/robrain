set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/avcodec/ts/test)

include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/avcodec/ts/src
)

link_directories(
)


#################################
add_executable(TestTsEncoder TestTsEncoder.cpp)
target_link_libraries(TestTsEncoder tscodec base pthread)

