add_executable(ExpectedTest ExpectedTests.cpp)
target_link_libraries(ExpectedTest PRIVATE CONAN_PKG::GTest CloveExpected)
add_test(NAME ExpectedTest COMMAND ExpectedTest)