set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/platform/linux)

include_directories(
    ${TOPDIR}/base/include
    ${TOPDIR}/opensource/prebuild/include/x264
)

link_directories(
    ${TOPDIR}/opensource/prebuild/libs
)


#################################
add_executable(robrain main.c rb_h264encode.c rb_video.c)

target_link_libraries(robrain base x264 pthread)