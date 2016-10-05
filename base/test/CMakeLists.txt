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