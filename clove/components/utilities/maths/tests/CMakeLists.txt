find_package(GTest REQUIRED CONFIG)

#Vector
add_executable(VectorTest VectorTests.cpp)
target_link_libraries(VectorTest PRIVATE GTest::gtest_main CloveMaths)
add_test(NAME VectorTest COMMAND VectorTest)

#Matrix
add_executable(MatrixTest MatrixTests.cpp)
target_link_libraries(MatrixTest PRIVATE GTest::gtest_main CloveMaths)
add_test(NAME MatrixTest COMMAND MatrixTest)