set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/base/test)

include_directories(
    ${TOPDIR}/base/include
)



#################################
add_executable(TestIniFile TestIniFile.cpp)
target_link_libraries(TestIniFile base)


#################################
add_executable(TestLock TestLock.cpp)
target_link_libraries(TestLock base pthread)

#################################
add_executable(TestMemoryPool TestMemoryPool.cpp)
target_link_libraries(TestMemoryPool base)


#################################
add_executable(TestLog TestLog.cpp)
target_link_libraries(TestLog base)


#################################
add_executable(TestDumpFile TestDumpFile.cpp)
target_link_libraries(TestDumpFile base)


#################################
add_executable(TestDoubleLinkList TestDoubleLinkList.cpp)
target_link_libraries(TestDoubleLinkList base)


#################################
add_executable(TestThreadPool TestThreadPool.cpp)
target_link_libraries(TestThreadPool base pthread)


#################################
add_executable(TestThreadLoop TestThreadLoop.cpp)
target_link_libraries(TestThreadLoop base pthread)

