#include <Clove/Maths/Matrix.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(MatrixTests, CanCreateDefaultMatrix) {
    mat4f matrix{};

    for(size_t i{ 0 }; i < 4; ++i) {
        for(size_t j{ 0 }; j < 4; ++j) {
            EXPECT_EQ(matrix[i][j], 0.0f);
        }
    }
}

TEST(MatrixTests, MemoryIsColumnMajor) {
    mat4f matrix{};

    matrix[0] = { 1, 0, 4, 2 };
    matrix[1] = { 0, 1, 3, 2 };
    matrix[2] = { 0, 6, 1, 2 };
    matrix[3] = { 0, 7, 0, 1 };

    EXPECT_EQ(matrix[0][2], 4);
    EXPECT_EQ(matrix[3][1], 7);

    EXPECT_EQ(matrix[0][3], 2);
    EXPECT_EQ(matrix[1][3], 2);
    EXPECT_EQ(matrix[2][3], 2);

    EXPECT_EQ(matrix.data[12], 2);
    EXPECT_EQ(matrix.data[13], 2);
    EXPECT_EQ(matrix.data[14], 2);
}

TEST(MatrixTests, CanCreateIdentityMatrix) {
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

TEST(MatrixTests, CanMultiplyMatrixByVector) {
    {
        mat4f matrix{};
        matrix[0] = { 1, 0, 0, 0 };
        matrix[1] = { 0, 2, 4, 1 };
        matrix[2] = { 0, 0, 1, 0 };
        matrix[3] = { 9, 8, 1, 3 };

        vec4f const vector{ 2, 3, 4, 5 };

        vec4f const result{ matrix * vector };

        EXPECT_EQ(result[0], 2);
        EXPECT_EQ(result[1], 27);
        EXPECT_EQ(result[2], 4);
        EXPECT_EQ(result[3], 61);
    }

    //Simulate translating a vector
    {
        mat4f matrix{};
        matrix[0] = { 1, 0, 0, 2 };
        matrix[1] = { 0, 1, 0, 1 };
        matrix[2] = { 0, 0, 1, 3 };
        matrix[3] = { 0, 0, 0, 1 };

        vec4f const vector{ 2, 3, 4, 1 };

        vec4f const result{ matrix * vector };

        EXPECT_EQ(result[0], 4);
        EXPECT_EQ(result[1], 4);
        EXPECT_EQ(result[2], 7);
        EXPECT_EQ(result[3], 1);
    }
}

TEST(MatrixTests, CanMultiplyMatrixByMatrix) {
    //Test for correct row / colum sizing
    {
        mat<2, 3, float> a{};
        a[0] = { 0, 3, 5 };
        a[1] = { 5, 5, 2 };

        mat<3, 2, float> b{};
        b[0] = { 3, 4 };
        b[1] = { 3, -2 };
        b[2] = { 4, -2 };

        mat<2, 2, float> result{ a * b };

        EXPECT_EQ(result[0][0], 29);
        EXPECT_EQ(result[0][1], -16);
        EXPECT_EQ(result[1][0], 38);
        EXPECT_EQ(result[1][1], 6);
    }

    //Test for large matrices
    {
        mat4f a{};
        a[0] = { 0, 3, 5, 3 };
        a[1] = { 1, 7, 2, 2 };
        a[2] = { 5, 1, 2, 7 };
        a[3] = { 3, 2, 2, 9 };

        mat4f b{};
        b[0] = { 3, 4, 6, 5 };
        b[1] = { 3, -2, 8, 1 };
        b[2] = { 8, -6, 9, 4 };
        b[3] = { 4, -2, 1, 6 };

        mat4f result{ a * b };

        EXPECT_EQ(result[0][0], 61);
        EXPECT_EQ(result[0][1], -42);
        EXPECT_EQ(result[0][2], 72);
        EXPECT_EQ(result[0][3], 41);

        EXPECT_EQ(result[1][0], 48);
        EXPECT_EQ(result[1][1], -26);
        EXPECT_EQ(result[1][2], 82);
        EXPECT_EQ(result[1][3], 32);

        EXPECT_EQ(result[2][0], 62);
        EXPECT_EQ(result[2][1], -8);
        EXPECT_EQ(result[2][2], 63);
        EXPECT_EQ(result[2][3], 76);

        EXPECT_EQ(result[3][0], 67);
        EXPECT_EQ(result[3][1], -22);
        EXPECT_EQ(result[3][2], 61);
        EXPECT_EQ(result[3][3], 79);
    }
}

TEST(MatrixTests, CanTransposeAMatrix) {
    mat4f matrix{};

    matrix[0] = { 0, 1, 2, 3 };
    matrix[1] = { 4, 4, 4, 4 };
    matrix[2] = { 6, 7, 6, 7 };
    matrix[3] = { 1, 1, 0, 0 };

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

TEST(MatrixTests, CanInverseAMatrix) {
    {
        mat3f const aIdentity{ 1 };

        mat3f a{};
        a[0] = { 3, 0, 2 };
        a[1] = { 2, 0, -2 };
        a[2] = { 0, 1, 1 };

        ASSERT_NE(a, aIdentity);

        auto aInv{ inverse(a) };

        EXPECT_EQ(a * aInv, aIdentity);
        EXPECT_EQ(aInv * a, aIdentity);
    }

    {
        mat4f const bIdentity{ 1 };

        mat4f b{};
        b[0] = { 2, 0, 0, 0 };
        b[1] = { 0, 2, 0, 0 };
        b[2] = { 0, 0, 2, 0 };
        b[3] = { 0, 0, 0, 1 };

        ASSERT_NE(b, bIdentity);

        auto bInv{ inverse(b) };

        EXPECT_EQ(b * bInv, bIdentity);
        EXPECT_EQ(bInv * b, bIdentity);
    }

    //TODO: Test for detereminant 0
}

TEST(MatrixTests, CanTranslateAMatrix) {
    mat4f const m{ 1 };
    vec3f const v{ 3, 7, 8 };

    mat4f const translation{ translate(m, v) };
    mat4f result;
    result[0] = { 1, 0, 0, v.x };
    result[1] = { 0, 1, 0, v.y };
    result[2] = { 0, 0, 1, v.z };
    result[3] = { 0, 0, 0, 1 };

    EXPECT_EQ(translation[3], result[3]);

    EXPECT_EQ(translation[0][0], 1);
    EXPECT_EQ(translation[0][1], 0);
    EXPECT_EQ(translation[0][2], 0);
    EXPECT_EQ(translation[0][3], v.x);

    EXPECT_EQ(translation[1][0], 0);
    EXPECT_EQ(translation[1][1], 1);
    EXPECT_EQ(translation[1][2], 0);
    EXPECT_EQ(translation[1][3], v.y);

    EXPECT_EQ(translation[2][0], 0);
    EXPECT_EQ(translation[2][1], 0);
    EXPECT_EQ(translation[2][2], 1);
    EXPECT_EQ(translation[2][3], v.z);

    EXPECT_EQ(translation[3][0], 0);
    EXPECT_EQ(translation[3][1], 0);
    EXPECT_EQ(translation[3][2], 0);
    EXPECT_EQ(translation[3][3], 1);
}

TEST(MatrixTests, CanRotateAMatrix) {
    mat4f const m{ 1 };
    vec3f const x{ 1, 0, 0 };
    vec3f const y{ 0, 1, 0 };
    vec3f const z{ 0, 0, 1 };
    float const angle{ 45.0f };

    //X
    {
        mat4f const rotationX{ rotate(m, angle, x) };

        EXPECT_FLOAT_EQ(rotationX[0][0], 1);
        EXPECT_FLOAT_EQ(rotationX[0][1], 0);
        EXPECT_FLOAT_EQ(rotationX[0][2], 0);
        EXPECT_FLOAT_EQ(rotationX[0][3], 0);

        EXPECT_FLOAT_EQ(rotationX[1][0], 0);
        EXPECT_FLOAT_EQ(rotationX[1][1], std::cos(angle));
        EXPECT_FLOAT_EQ(rotationX[1][2], -std::sin(angle));
        EXPECT_FLOAT_EQ(rotationX[1][3], 0);

        EXPECT_FLOAT_EQ(rotationX[2][0], 0);
        EXPECT_FLOAT_EQ(rotationX[2][1], std::sin(angle));
        EXPECT_FLOAT_EQ(rotationX[2][2], std::cos(angle));
        EXPECT_FLOAT_EQ(rotationX[2][3], 0);

        EXPECT_FLOAT_EQ(rotationX[3][0], 0);
        EXPECT_FLOAT_EQ(rotationX[3][1], 0);
        EXPECT_FLOAT_EQ(rotationX[3][2], 0);
        EXPECT_FLOAT_EQ(rotationX[3][3], 1);
    }

    //Y
    {
        mat4f const rotationY{ rotate(m, angle, y) };

        EXPECT_FLOAT_EQ(rotationY[0][0], std::cos(angle));
        EXPECT_FLOAT_EQ(rotationY[0][1], 0);
        EXPECT_FLOAT_EQ(rotationY[0][2], std::sin(angle));
        EXPECT_FLOAT_EQ(rotationY[0][3], 0);

        EXPECT_FLOAT_EQ(rotationY[1][0], 0);
        EXPECT_FLOAT_EQ(rotationY[1][1], 1);
        EXPECT_FLOAT_EQ(rotationY[1][2], 0);
        EXPECT_FLOAT_EQ(rotationY[1][3], 0);

        EXPECT_FLOAT_EQ(rotationY[2][0], -std::sin(angle));
        EXPECT_FLOAT_EQ(rotationY[2][1], 0);
        EXPECT_FLOAT_EQ(rotationY[2][2], std::cos(angle));
        EXPECT_FLOAT_EQ(rotationY[2][3], 0);

        EXPECT_FLOAT_EQ(rotationY[3][0], 0);
        EXPECT_FLOAT_EQ(rotationY[3][1], 0);
        EXPECT_FLOAT_EQ(rotationY[3][2], 0);
        EXPECT_FLOAT_EQ(rotationY[3][3], 1);
    }

    //Z
    {
        mat4f const rotationZ{ rotate(m, angle, z) };

        EXPECT_FLOAT_EQ(rotationZ[0][0], std::cos(angle));
        EXPECT_FLOAT_EQ(rotationZ[0][1], -std::sin(angle));
        EXPECT_FLOAT_EQ(rotationZ[0][2], 0);
        EXPECT_FLOAT_EQ(rotationZ[0][3], 0);

        EXPECT_FLOAT_EQ(rotationZ[1][0], std::sin(angle));
        EXPECT_FLOAT_EQ(rotationZ[1][1], std::cos(angle));
        EXPECT_FLOAT_EQ(rotationZ[1][2], 0);
        EXPECT_FLOAT_EQ(rotationZ[1][3], 0);

        EXPECT_FLOAT_EQ(rotationZ[2][0], 0);
        EXPECT_FLOAT_EQ(rotationZ[2][1], 0);
        EXPECT_FLOAT_EQ(rotationZ[2][2], 1);
        EXPECT_FLOAT_EQ(rotationZ[2][3], 0);

        EXPECT_FLOAT_EQ(rotationZ[3][0], 0);
        EXPECT_FLOAT_EQ(rotationZ[3][1], 0);
        EXPECT_FLOAT_EQ(rotationZ[3][2], 0);
        EXPECT_FLOAT_EQ(rotationZ[3][3], 1);
    }
}