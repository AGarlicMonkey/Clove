find_package(GTest REQUIRED CONFIG)

#Blackboard
add_executable(BlackboardTest BlackboardTests.cpp)
target_link_libraries(BlackboardTest PRIVATE GTest::gtest_main CloveBlackboard)
add_test(NAME BlackboardTest COMMAND BlackboardTest)