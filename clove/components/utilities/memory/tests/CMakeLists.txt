#StackAllocator
add_executable(StackAllocatorTest StackAllocatorTests.cpp)
target_link_libraries(StackAllocatorTest PRIVATE CONAN_PKG::GTest CloveMemory)
add_test(NAME StackAllocatorTest COMMAND StackAllocatorTest)

#ListAllocator
add_executable(ListAllocatorTest ListAllocatorTests.cpp)
target_link_libraries(ListAllocatorTest PRIVATE CONAN_PKG::GTest CloveMemory)
add_test(NAME ListAllocatorTest COMMAND ListAllocatorTest)

#PoolAllocator
add_executable(PoolAllocatorTest PoolAllocatorTests.cpp)
target_link_libraries(PoolAllocatorTest PRIVATE CONAN_PKG::GTest CloveMemory)
add_test(NAME PoolAllocatorTest COMMAND PoolAllocatorTest)