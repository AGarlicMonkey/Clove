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