find_package(GTest REQUIRED CONFIG)

#Node
add_executable(NodeTest NodeTests.cpp)
target_link_libraries(NodeTest PRIVATE GTest::gtest_main CloveSerialisation)
add_test(NAME NodeTest COMMAND NodeTest)

#Serialisation
add_executable(YamlSerialisationTest YamlSerialisationTests.cpp)
target_link_libraries(YamlSerialisationTest PRIVATE GTest::gtest_main CloveSerialisation)
add_test(NAME YamlSerialisationTest COMMAND YamlSerialisationTest)

#Deerialisation
add_executable(YamlDeserialisationTest YamlDeserialisationTests.cpp)
target_link_libraries(YamlDeserialisationTest PRIVATE GTest::gtest_main CloveSerialisation)
add_test(NAME YamlDeserialisationTest COMMAND YamlDeserialisationTest)