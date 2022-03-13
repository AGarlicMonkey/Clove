#include <Clove/Maths/Quaternion.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(QuaternionTests, CanDefaultConstructAQuaternion) {
    quatf q{};

    EXPECT_FLOAT_EQ(q.x, 0);
    EXPECT_FLOAT_EQ(q.y, 0);
    EXPECT_FLOAT_EQ(q.z, 0);
    EXPECT_FLOAT_EQ(q.w, 1);
}