find_package(GTest REQUIRED CONFIG)

add_executable(EventTest EventTests.cpp)
target_link_libraries(EventTest PRIVATE GTest::gtest_main CloveEvent)
add_test(NAME EventTest COMMAND EventTest)