#Entity
add_executable(ECSEntityTest EntityTests.cpp)
target_link_libraries(ECSEntityTest PRIVATE CONAN_PKG::gtest CloveECS)
add_test(NAME ECSEntityTest COMMAND ECSEntityTest)

#Component
add_executable(ECSComponentTest ComponentTests.cpp)
target_link_libraries(ECSComponentTest PRIVATE CONAN_PKG::gtest CloveECS)
add_test(NAME ECSComponentTest COMMAND ECSComponentTest)

#System
add_executable(ECSSystemTest SystemTests.cpp)
target_link_libraries(ECSSystemTest PRIVATE CONAN_PKG::gtest CloveECS)
add_test(NAME ECSSystemTest COMMAND ECSSystemTest)