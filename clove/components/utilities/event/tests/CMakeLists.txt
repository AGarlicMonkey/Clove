add_executable(EventTest EventTests.cpp)
target_link_libraries(EventTest PRIVATE CONAN_PKG::gtest CloveEvent)
add_test(NAME EventTest COMMAND EventTest)