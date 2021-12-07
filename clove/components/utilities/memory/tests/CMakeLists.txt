find_package(GTest REQUIRED CONFIG)

#StackAllocator
add_executable(StackAllocatorTest StackAllocatorTests.cpp)
target_link_libraries(StackAllocatorTest PRIVATE GTest::gtest_main CloveMemory)
add_test(NAME StackAllocatorTest COMMAND StackAllocatorTest)

#ListAllocator
add_executable(ListAllocatorTest ListAllocatorTests.cpp)
target_link_libraries(ListAllocatorTest PRIVATE GTest::gtest_main CloveMemory)
add_test(NAME ListAllocatorTest COMMAND ListAllocatorTest)

#PoolAllocator
add_executable(PoolAllocatorTest PoolAllocatorTests.cpp)
target_link_libraries(PoolAllocatorTest PRIVATE GTest::gtest_main CloveMemory)
add_test(NAME PoolAllocatorTest COMMAND PoolAllocatorTest)