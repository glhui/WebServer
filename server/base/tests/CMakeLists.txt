function(add_test_executable target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target}
            PRIVATE
            libserver_base
    )
    # 可选：添加到 CTest 测试列表中
    add_test(NAME ${target} COMMAND ${target})
endfunction()

# 使用示例
#add_test_executable(MutexLockTest MutexLockTest.cpp)
#add_test_executable(ConditionTest ConditionTest.cpp)
#add_test_executable(CountDownLatchTest CountDownLatchTest.cpp)
#add_test_executable(FileUtilTest FileUtilTest.cpp)
#add_test_executable(ThreadTest ThreadTest.cpp)
add_test_executable(AsyncLoggingTest AsyncLoggingTest.cpp)