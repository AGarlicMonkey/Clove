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

TEST(QuaternionTests, CanCompareTwoQuaternions) {
    quatf a{ 4, 1, 1, 0 };
    quatf b{ 4, 1, 1, 0 };
    quatf c{ 9, 2, 5, 8 };

    EXPECT_TRUE(a == b);
    EXPECT_TRUE(b != c);

    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a != b);
}

TEST(QuaternionTests, CanAddTwoQuaternions) {
    quatf const a{ 9, 1, 0, 6 };
    quatf const b{ 2, 1, 2, 1 };
    quatf c{ a };

    quatf const v4sum{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    EXPECT_EQ(a + b, v4sum);

    c += b;
    EXPECT_EQ(c.x, a.x + b.x);
    EXPECT_EQ(c.y, a.y + b.y);
    EXPECT_EQ(c.z, a.z + b.z);
    EXPECT_EQ(c.w, a.w + b.w);
}

TEST(QuaternionTests, CanSubtractTwoQuaternions) {
    quatf const a{ 1, 2, 1, 1 };
    quatf const b{ 5, 6, 2, 0 };
    quatf c{ a };

    quatf const sub{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    EXPECT_EQ(a - b, sub);

    c -= b;
    EXPECT_EQ(c.x, a.x - b.x);
    EXPECT_EQ(c.y, a.y - b.y);
    EXPECT_EQ(c.z, a.z - b.z);
    EXPECT_EQ(c.w, a.w - b.w);
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

TEST(QuaternionTests, CanDotProductTwoQuaternions) {
    quatf const a{ 1, 2, 1, 4 };
    quatf const b{ 4, 1, 3, 9 };

    const float qDot{ dot(a, b) };
    const float qDotTest{ a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w };
    EXPECT_FLOAT_EQ(qDot, qDotTest);
}

TEST(QuaternionTests, CanLerpBetweenTwoQuaternions) {
    quatf const a{ 0.0f, 0.0f, 0.0f, 0.0f };
    quatf const b{ 1.0f, 1.0f, 1.0f, 1.0f };

    quatf const res0{ lerp(a, b, 0.0f) };
    EXPECT_EQ(res0, a);

    quatf const res1{ lerp(a, b, 1.0f) };
    EXPECT_EQ(res1, b);

    quatf const res2{ lerp(a, b, 0.5f) };
    EXPECT_EQ(res2.x, 0.5f);
    EXPECT_EQ(res2.y, 0.5f);
    EXPECT_EQ(res2.z, 0.5f);
    EXPECT_EQ(res2.w, 0.5f);
}

TEST(QuaternionTests, CanSlerpBetweenTwoQuaternions) {
    quatf const a{ 0.0f, 0.0f, 0.0f, 0.0f };
    quatf const b{ 1.0f, 1.0f, 1.0f, 1.0f };

    quatf const res0{ slerp(a, b, 0.0f) };
    EXPECT_EQ(res0, a);

    quatf const res1{ slerp(a, b, 1.0f) };
    EXPECT_EQ(res1, b);

    quatf const res2{ slerp(a, b, 0.5f) };
    EXPECT_FLOAT_EQ(res2.x, 0.707107f);
    EXPECT_FLOAT_EQ(res2.y, 0.707107f);
    EXPECT_FLOAT_EQ(res2.z, 0.707107f);
    EXPECT_FLOAT_EQ(res2.w, 0.707107f);
}