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

TEST(QuaternionTest, CanCompareTwoQuaternions) {
    quatf a{ 4, 1, 1, 0 };
    quatf b{ 4, 1, 1, 0 };
    quatf c{ 9, 2, 5, 8 };

    EXPECT_TRUE(a == b);
    EXPECT_TRUE(b != c);

    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a != b);
}

TEST(QuaternionTests, CanMultiplyQuaternionByNumber) {
    float constexpr multiplier{ 5.0f };

    quatf a{ 2, 1, 0, 1 };
    quatf aMul{ a.x * multiplier, a.y * multiplier, a.z * multiplier, a.w * multiplier };
    EXPECT_EQ(a * multiplier, aMul);

    quatf b{ 1, 9, 8, 2 };
    quatf c{ b };

    c *= multiplier;
    EXPECT_EQ(b.x * multiplier, c.x);
    EXPECT_EQ(b.y * multiplier, c.y);
    EXPECT_EQ(b.z * multiplier, c.z);
    EXPECT_EQ(b.w * multiplier, c.w);
}

TEST(QuaternionTests, CanDivideQuaternionByNumber) {
    float constexpr divisor{ 3.0f };

    quatf a{ 3, 2, 1, 6 };
    quatf b{ 2, 1, 3, 6 };
    quatf c{ b };

    quatf aDiv{ a.x / divisor, a.y / divisor, a.z / divisor, a.w / divisor };
    EXPECT_EQ(a / divisor, aDiv);

    b /= divisor;
    EXPECT_EQ(b.x, c.x / divisor);
    EXPECT_EQ(b.y, c.y / divisor);
    EXPECT_EQ(b.z, c.z / divisor);
    EXPECT_EQ(b.w, c.w / divisor);
}

TEST(QuaternionTests, CanGetLengthOfAQuaternion) {
    quatf const q{ 1, 3, 2, 5 };

    EXPECT_FLOAT_EQ(length(q), std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w));
}

TEST(QuaternionTests, CanNormaliseAQuaternion) {
    quatf const q{ 4, 3, 8, 5 };

    EXPECT_FLOAT_EQ(length(normalise(q)), 1.0f);
}