#include <Clove/Maths/Matrix.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(MatrixTests, CanCreateDefaultMatrix) {
    mat4f matrix{};

    for(size_t i{ 0 }; i < 4; ++i){
        for(size_t j{ 0 }; j < 4; ++j){
            EXPECT_EQ(matrix.rows[i][j], 0.0f);
        }
    }
}

TEST(MatrixTests, CanUseIndexOperator) {
    mat4f matrix{};

    for(size_t i{ 0 }; i < 4; ++i) {
        for(size_t j{ 0 }; j < 4; ++j) {
            EXPECT_EQ(matrix.rows[i][j], matrix[i][j]);
        }
    }

    EXPECT_EQ(matrix[3][3], matrix[32][99]);
}