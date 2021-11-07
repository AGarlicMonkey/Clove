#Vector
add_executable(VectorTest VectorTests.cpp)
target_link_libraries(VectorTest PRIVATE CONAN_PKG::gtest CloveMaths)
add_test(NAME VectorTest COMMAND VectorTestTest)