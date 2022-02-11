find_package(GTest REQUIRED CONFIG)

add_executable(GuidTest GuidTests.cpp)
target_link_libraries(GuidTest PRIVATE GTest::gtest_main CloveGuid)
add_test(NAME GuidTest COMMAND GuidTest)