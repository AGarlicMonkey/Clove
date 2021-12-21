#include <Clove/Maths/Vector.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(VectorTests, CanDefaultConstructAVector) {
    vec<1, float> v1{};

    EXPECT_EQ(v1.x, float{});
    EXPECT_EQ(v1.r, float{});
    EXPECT_EQ(v1.u, float{});

    vec<2, float> v2{};

    EXPECT_EQ(v2.x, float{});
    EXPECT_EQ(v2.r, float{});
    EXPECT_EQ(v2.u, float{});

    EXPECT_EQ(v2.y, float{});
    EXPECT_EQ(v2.g, float{});
    EXPECT_EQ(v2.v, float{});

    vec<3, float> v3{};

    EXPECT_EQ(v3.x, float{});
    EXPECT_EQ(v3.r, float{});

    EXPECT_EQ(v3.y, float{});
    EXPECT_EQ(v3.g, float{});

    EXPECT_EQ(v3.z, float{});
    EXPECT_EQ(v3.b, float{});

    vec<4, float> v4{};

    EXPECT_EQ(v4.x, float{});
    EXPECT_EQ(v4.r, float{});

    EXPECT_EQ(v4.y, float{});
    EXPECT_EQ(v4.g, float{});

    EXPECT_EQ(v4.z, float{});
    EXPECT_EQ(v4.b, float{});

    EXPECT_EQ(v4.w, float{});
    EXPECT_EQ(v4.a, float{});
}

TEST(VectorTests, CanAccessMembersByIndex) {
    vec1f v1{ 0 };

    EXPECT_EQ(v1.x, v1[0]);

    EXPECT_EQ(v1.x, v1[1]);

    vec2f v2{ 0, 1 };

    EXPECT_EQ(v2.x, v2[0]);
    EXPECT_EQ(v2.y, v2[1]);

    EXPECT_EQ(v2.y, v2[2]);

    vec3f v3{ 0, 1, 2 };

    EXPECT_EQ(v3.x, v3[0]);
    EXPECT_EQ(v3.y, v3[1]);
    EXPECT_EQ(v3.z, v3[2]);

    EXPECT_EQ(v3.z, v3[3]);

    vec4f v4{ 0, 1, 2, 3 };

    EXPECT_EQ(v4.x, v4[0]);
    EXPECT_EQ(v4.y, v4[1]);
    EXPECT_EQ(v4.z, v4[2]);
    EXPECT_EQ(v4.w, v4[3]);

    EXPECT_EQ(v4.w, v4[4]);
}

TEST(VectorTests, CanGetLengthOfAVector) {
    vec1f v1{ 1 };

    EXPECT_EQ(length(v1), std::sqrt(v1.x));

    vec2f v2{ 1, 2 };

    EXPECT_EQ(length(v2), std::sqrt(v2.x + v2.y));

    vec3f v3{ 2, 2, 2 };

    EXPECT_EQ(length(v3), std::sqrt(v3.x + v3.y + v3.z));

    vec4f v4{ 3, 2, 1, 6 };

    EXPECT_EQ(length(v4), std::sqrt(v4.x + v4.y + v4.z + v4.w));
}
