#include <Clove/Maths/Matrix.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(MatrixTests, CanCreateDefaultMatrix) {
    mat4f matrix{};

    for(size_t i{ 0 }; i < 4; ++i){
        for(size_t j{ 0 }; j < 4; ++j){
            EXPECT_EQ(matrix.rows[i][j], 0);
        }
    }
}
