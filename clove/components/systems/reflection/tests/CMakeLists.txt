find_package(GTest REQUIRED CONFIG)

#Reflection
add_executable(ReflectionTest ReflectionTests.cpp)
target_link_libraries(ReflectionTest PRIVATE GTest::gtest_main CloveReflection)
add_test(NAME ReflectionTest COMMAND ReflectionTest)