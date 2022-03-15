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

TEST(QuaternionTests, CanGetLengthOfAQuaternion) {
    quatf q{ 1, 3, 2, 5 };

    EXPECT_FLOAT_EQ(length(q), std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w));
}
