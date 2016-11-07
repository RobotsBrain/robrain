set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/avcodec/flv/test)

include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/avcodec/flv/src
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestFlvParser TestFlvParser.cpp)
target_link_libraries(TestFlvParser flvcodec base pthread)


#################################
add_executable(TestFlvEncoder TestFlvEncoder.cpp)
target_link_libraries(TestFlvEncoder flvcodec base pthread)