find_package(GTest REQUIRED CONFIG)

#Specify working directory to allow the tests to find any DLLs Clove requires
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

add_executable(AssetPtrTest AssetPtrTests.cpp)
target_link_libraries(AssetPtrTest PRIVATE GTest::gtest_main Clove)
add_test(NAME AssetPtrTest COMMAND AssetPtrTest WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<${isMultiConfig}:/$<CONFIG>>)

add_executable(FileSystemVFSTest FileSystemVFSTests.cpp)
target_link_libraries(FileSystemVFSTest PRIVATE GTest::gtest_main Clove)
add_test(NAME FileSystemVFSTest COMMAND FileSystemVFSTest WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<${isMultiConfig}:/$<CONFIG>>)