find_package(GTest REQUIRED CONFIG)

#Entity
add_executable(ECSEntityTest EntityTests.cpp)
target_link_libraries(ECSEntityTest PRIVATE GTest::gtest_main CloveECS)
add_test(NAME ECSEntityTest COMMAND ECSEntityTest)

#Component
add_executable(ECSComponentTest ComponentTests.cpp)
target_link_libraries(ECSComponentTest PRIVATE GTest::gtest_main CloveECS)
add_test(NAME ECSComponentTest COMMAND ECSComponentTest)

#System
add_executable(ECSSystemTest SystemTests.cpp)
target_link_libraries(ECSSystemTest PRIVATE GTest::gtest_main CloveECS)
add_test(NAME ECSSystemTest COMMAND ECSSystemTest)