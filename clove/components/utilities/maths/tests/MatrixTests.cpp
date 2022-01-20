#include <Clove/Maths/Matrix.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(MatrixTests, CanCreateDefaultMatrix) {
    mat4f matrix{};

    for(size_t i{ 0 }; i < 4; ++i){
        for(size_t j{ 0 }; j < 4; ++j){
            EXPECT_EQ(matrix.value[i][j], 0.0f);
        }
    }
}

TEST(MatrixTests, CanUseIndexOperator) {
    mat4f matrix{};

    for(size_t i{ 0 }; i < 4; ++i) {
        for(size_t j{ 0 }; j < 4; ++j) {
            EXPECT_EQ(matrix.value[i][j], matrix[i][j]);
        }
    }

    EXPECT_EQ(matrix[3][3], matrix[32][99]);
}

TEST(MatrixTests, CanCreateIdentityMatrix){
    mat4f identity{ 1.0f };

    EXPECT_EQ(identity[0][0], 1.0f);
    EXPECT_NE(identity[0][1], 1.0f);
    EXPECT_NE(identity[0][2], 1.0f);
    EXPECT_NE(identity[0][3], 1.0f);

    EXPECT_NE(identity[1][0], 1.0f);
    EXPECT_EQ(identity[1][1], 1.0f);
    EXPECT_NE(identity[1][2], 1.0f);
    EXPECT_NE(identity[1][3], 1.0f);

    EXPECT_NE(identity[2][0], 1.0f);
    EXPECT_NE(identity[2][1], 1.0f);
    EXPECT_EQ(identity[2][2], 1.0f);
    EXPECT_NE(identity[2][3], 1.0f);

    EXPECT_NE(identity[3][0], 1.0f);
    EXPECT_NE(identity[3][1], 1.0f);
    EXPECT_NE(identity[3][2], 1.0f);
    EXPECT_EQ(identity[3][3], 1.0f);
}

