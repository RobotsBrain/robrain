set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/avcodec/aac/test)

include_directories(
	${TOPDIR}/basics/base/include
	${TOPDIR}/avcodec/aac/src
	${TOPDIR}/opensource/prebuild/include/faac
)

link_directories(
	${TOPDIR}/opensource/prebuild/libs/${OPENSOURCE_LIB_DIR}
)


#################################
add_executable(TestAacEncoder TestAacEncoder.cpp)
target_link_libraries(TestAacEncoder faac)


#################################
add_executable(TestAacHeaderParser TestAacHeaderParser.cpp)
target_link_libraries(TestAacHeaderParser aaccodec)