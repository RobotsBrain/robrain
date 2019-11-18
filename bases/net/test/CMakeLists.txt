set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/network/test)

include_directories(
    ${TOPDIR}/network/include
    ${TOPDIR}/basics/base/include
    ${TOPDIR}/opensource/prebuild/include/pcap
    ${TOPDIR}/opensource/prebuild/include/adns
    ${TOPDIR}/opensource/prebuild/include/libev
)

link_directories(
    ${TOPDIR}/opensource/prebuild/libs
)

#target_link_libraries(testpcap adns pcap dbus-1)

#################################
add_executable(TestLnwDetail TestLnwDetail.cpp)
target_link_libraries(TestLnwDetail network)


#################################
add_executable(TestDnsResolver TestDnsResolver.cpp)
target_link_libraries(TestDnsResolver network adns)


#################################
add_executable(TestLibev TestLibev.cpp)
target_link_libraries(TestLibev base ev)


