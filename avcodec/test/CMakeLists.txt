set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/avcodec/test)

include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/avcodec/src
	${TOPDIR}/opensource/prebuild/include/mp4v2/include
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestMp4Encoder TestMp4Encoder.cpp)
target_link_libraries(TestMp4Encoder avcodec mp4v2 base pthread)

