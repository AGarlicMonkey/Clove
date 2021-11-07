#pragma once

#include "Clove/Maths/MathsTypes.hpp"

namespace clove {
    // template<length_t L, typename T>
    // vec<L, T> normalise(vec<L, T> const &v);

    // template<typename T>
    // quat<T> normalise(quat<T> const &q);

    // template<typename T>
    // vec<3, T> cross(vec<3, T> const &v1, vec<3, T> const &v2);

    // template<length_t L, typename T>
    // T dot(vec<L, T> const &v1, vec<L, T> const &v2);

    // template<length_t L, typename T>
    // T distance(vec<L, T> const &v1, vec<L, T> const &v2);

    // template<length_t L, typename T>
    // T distanceSquared(vec<L, T> const &v1, vec<L, T> const &v2);

    // template<length_t L, typename T>
    // T length(vec<L, T> const &vec);

    // template<length_t L, typename T>
    // T lengthSquared(vec<L, T> const &vec);

    // template<typename T>
    // mat<4, 4, T> lookAt(vec<3, T> const &eye, vec<3, T> const &center, vec<3, T> const &worldUp);

    // template<typename T>
    // mat<4, 4, T> translate(mat<4, 4, T> const &m, vec<3, T> const &v);

    // template<typename T>
    // mat<4, 4, T> rotate(mat<4, 4, T> const &m, T angle, vec<3, T> const &v);

    // template<typename T>
    // quat<T> constexpr rotate(T angle, vec<3, T> const &v);

    // template<typename T>
    // mat<4, 4, T> scale(mat<4, 4, T> const &m, vec<3, T> const &v);

    // template<length_t C, length_t R, typename T>
    // mat<C, R, T> inverse(mat<C, R, T> const &m);

    // template<length_t C, length_t R, typename T>
    // mat<C, R, T> transpose(mat<C, R, T> const &m);

    // //Linear interpolate between two scalars
    // template<typename T>
    // T lerp(T const &a, T const &b, T t);

    // //Linear interpolate between two vectors
    // template<length_t L, typename T>
    // vec<L, T> lerp(vec<L, T> const &a, vec<L, T> const &b, T t);

    // //Linear interpolate between two quaternions
    // template<typename T>
    // quat<T> lerp(quat<T> const &a, quat<T> const &b, T t);

    // //Spherical linear interpolate between two quaternions
    // template<typename T>
    // quat<T> slerp(quat<T> const &a, quat<T> const &b, T t);
}

#include "Maths.inl"