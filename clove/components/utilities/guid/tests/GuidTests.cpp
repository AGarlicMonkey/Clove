#include <Clove/Guid.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(GuidTests, CanGenerateAGuid) {
    Guid guid{};

    EXPECT_TRUE(guid != 0);
}

TEST(GuidTests, GuidsAreNotEqual) {
    Guid a{};
    Guid b{};

    EXPECT_NE(a, b);
}