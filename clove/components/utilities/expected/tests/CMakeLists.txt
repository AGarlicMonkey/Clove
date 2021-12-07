find_package(GTest REQUIRED CONFIG)

add_executable(ExpectedTest ExpectedTests.cpp)
target_link_libraries(ExpectedTest PRIVATE GTest::gtest_main CloveExpected)
add_test(NAME ExpectedTest COMMAND ExpectedTest)