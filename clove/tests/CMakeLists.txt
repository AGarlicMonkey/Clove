find_package(GTest REQUIRED CONFIG)

add_executable(AssetPtrTest AssetPtrTests.cpp)
target_link_libraries(AssetPtrTest PRIVATE GTest::gtest_main Clove)
add_test(NAME AssetPtrTest COMMAND AssetPtrTest)

add_executable(VirtualFileSystem VirtualFileSystemTests.cpp)
target_link_libraries(VirtualFileSystem PRIVATE GTest::gtest_main Clove)
add_test(NAME VirtualFileSystem COMMAND VirtualFileSystem)