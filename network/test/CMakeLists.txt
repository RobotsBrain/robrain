set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/network/test)

include_directories(
    ${TOPDIR}/network/include
    ${TOPDIR}/base/include
    ${TOPDIR}/opensource/prebuild/include/pcap
    ${TOPDIR}/opensource/prebuild/include/adns
)

link_directories(
    ${TOPDIR}/opensource/prebuild/libs
)

#target_link_libraries(testpcap adns pcap dbus-1)

#################################
add_executable(TestDnsResolver TestDnsResolver.cpp)
target_link_libraries(TestDnsResolver network adns)