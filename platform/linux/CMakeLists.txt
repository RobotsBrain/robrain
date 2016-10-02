set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/platform/linux)

include_directories(
    ${TOPDIR}/base/include
    ${TOPDIR}/opensource/prebuild/include/x264
)

link_directories(
    ${TOPDIR}/opensource/prebuild/libs
)


#################################
add_executable(robrain main.cpp VideoCapture.cpp X264Encoder.cpp DumpH264File.cpp)

target_link_libraries(robrain base x264 pthread)