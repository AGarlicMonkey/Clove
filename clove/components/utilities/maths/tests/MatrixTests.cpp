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

TEST(MatrixTests, CanTransposeAMatrix) {
    mat4f matrix{};

    matrix[0][0] = 0;
    matrix[0][1] = 1;
    matrix[0][2] = 2;
    matrix[0][3] = 3;

    matrix[1][0] = 4;
    matrix[1][1] = 4;
    matrix[1][2] = 4;
    matrix[1][3] = 4;

    matrix[2][0] = 6;
    matrix[2][1] = 7;
    matrix[2][2] = 6;
    matrix[2][3] = 7;

    matrix[3][0] = 1;
    matrix[3][1] = 1;
    matrix[3][2] = 0;
    matrix[3][3] = 0;

    mat4f const transposed{ transpose(matrix) };

    EXPECT_EQ(transposed[0][0], matrix[0][0]);
    EXPECT_EQ(transposed[0][1], matrix[1][0]);
    EXPECT_EQ(transposed[0][2], matrix[2][0]);
    EXPECT_EQ(transposed[0][3], matrix[3][0]);

    EXPECT_EQ(transposed[1][0], matrix[0][1]);
    EXPECT_EQ(transposed[1][1], matrix[1][1]);
    EXPECT_EQ(transposed[1][2], matrix[2][1]);
    EXPECT_EQ(transposed[1][3], matrix[3][1]);

    EXPECT_EQ(transposed[2][0], matrix[0][2]);
    EXPECT_EQ(transposed[2][1], matrix[1][2]);
    EXPECT_EQ(transposed[2][2], matrix[2][2]);
    EXPECT_EQ(transposed[2][3], matrix[3][2]);

    EXPECT_EQ(transposed[3][0], matrix[0][3]);
    EXPECT_EQ(transposed[3][1], matrix[1][3]);
    EXPECT_EQ(transposed[3][2], matrix[2][3]);
    EXPECT_EQ(transposed[3][3], matrix[3][3]);
}