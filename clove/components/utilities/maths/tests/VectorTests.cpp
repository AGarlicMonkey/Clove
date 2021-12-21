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

TEST(VectorTests, CanCompareTwoVectors){
    vec1f v1a{ 1 };
    vec1f v1b{ 1 };
    vec1f v1c{ 2 };

    EXPECT_TRUE(v1a == v1b);
    EXPECT_TRUE(v1b != v1c);

    EXPECT_FALSE(v1a == v1c);
    EXPECT_FALSE(v1a != v1b);

    vec2f v2a{ 1, 2 };
    vec2f v2b{ 1, 2 };
    vec2f v2c{ 4, 3 };

    EXPECT_TRUE(v2a == v2b);
    EXPECT_TRUE(v2b != v2c);

    EXPECT_FALSE(v2a == v2c);
    EXPECT_FALSE(v2a != v2b);

    vec3f v3a{ 2, 2, 2 };
    vec3f v3b{ 2, 2, 2 };
    vec3f v3c{ 4, 6, 7 };

    EXPECT_TRUE(v3a == v3b);
    EXPECT_TRUE(v3b != v3c);

    EXPECT_FALSE(v3a == v3c);
    EXPECT_FALSE(v3a != v3b);

    vec4f v4a{ 3, 2, 1, 6 };
    vec4f v4b{ 3, 2, 1, 6 };
    vec4f v4c{ 9, 44, 5, 2 };

    EXPECT_TRUE(v4a == v4b);
    EXPECT_TRUE(v4b != v4c);

    EXPECT_FALSE(v4a == v4c);
    EXPECT_FALSE(v4a != v4b);
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
