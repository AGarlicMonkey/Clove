find_package(GTest REQUIRED CONFIG)

#SingleCastDelegate
add_executable(SingleCastDelegateTest SingleCastDelegateTests.cpp)
target_link_libraries(SingleCastDelegateTest PRIVATE GTest::gtest_main CloveDelegate)
add_test(NAME SingleCastDelegateTest COMMAND SingleCastDelegateTest)

#MultiCastDelegate
add_executable(MultiCastDelegateTest MultiCastDelegateTests.cpp)
target_link_libraries(MultiCastDelegateTest PRIVATE GTest::gtest_main CloveDelegate)
add_test(NAME MultiCastDelegateTest COMMAND MultiCastDelegateTest)