set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/avcodec/mp4/test)

include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/avcodec/mp4/src
	${TOPDIR}/opensource/prebuild/include/mp4v2/include
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestMp4Encoder TestMp4Encoder.cpp)
target_link_libraries(TestMp4Encoder mp4codec mp4v2 base pthread)


#################################
add_executable(TestMp4Parser TestMp4Parser.cpp)
target_link_libraries(TestMp4Parser mp4codec mp4v2 base pthread)