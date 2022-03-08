find_package(GTest REQUIRED CONFIG)

#Specify working directory to allow the tests to find any DLLs Clove requires

add_executable(AssetPtrTest AssetPtrTests.cpp)
target_link_libraries(AssetPtrTest PRIVATE GTest::gtest_main Clove)
add_test(NAME AssetPtrTest COMMAND AssetPtrTest WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)

add_executable(VirtualFileSystem VirtualFileSystemTests.cpp)
target_link_libraries(VirtualFileSystem PRIVATE GTest::gtest_main Clove)
add_test(NAME VirtualFileSystem COMMAND VirtualFileSystem WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)