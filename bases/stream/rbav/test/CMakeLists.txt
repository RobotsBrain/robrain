set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/rbav/test)

include_directories(
	${TOPDIR}/rbav/include
)

link_directories(
)


#################################
add_executable(TestComposeStream TestComposeStream.cpp)
target_link_libraries(TestComposeStream rbav)


#################################
add_executable(TestRbavParser TestRbavParser.cpp)
target_link_libraries(TestRbavParser rbav)