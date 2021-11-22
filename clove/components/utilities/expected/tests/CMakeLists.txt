add_executable(ExpectedTest ExpectedTests.cpp)
target_link_libraries(ExpectedTest PRIVATE CONAN_PKG::gtest CloveExpected)
add_test(NAME ExpectedTest COMMAND ExpectedTest)