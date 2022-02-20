find_package(GTest REQUIRED CONFIG)

add_executable(AssetPtrTest AssetPtrTests.cpp)
target_link_libraries(AssetPtrTest PRIVATE GTest::gtest_main Clove)
add_test(NAME AssetPtrTest COMMAND AssetPtrTest)

add_executable(FileSystemVFSTest FileSystemVFSTests.cpp)
target_link_libraries(FileSystemVFSTest PRIVATE GTest::gtest_main Clove)
add_test(NAME FileSystemVFSTest COMMAND FileSystemVFSTest)