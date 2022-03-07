#pragma once

#include "Concepts.hpp"
#include "Vector.hpp"

#include <array>

//Matrix types
namespace clove {
    template<size_t R, size_t C, number T>
    struct mat {
        std::array<vec<C, T>, R> value{};

        constexpr mat() = default;
        constexpr mat(T val);

        template<size_t R, size_t C, /* number */ typename T> //TODO: should be number but compiler issue is preventing that
        friend constexpr vec<C, T> operator*(mat<R, C, T> const &m, vec<C, T> const &v);

        template<size_t R1, size_t R2, size_t C, /* number */ typename T>//TODO: should be number but compiler issue is preventing that
        friend constexpr mat<R1, R2, T> operator*(mat<R1, C, T> const &a, mat<C, R2, T> const &b);

        template<size_t R, size_t C, /* number */ typename T>//TODO: should be number but compiler issue is preventing that
        friend constexpr bool operator==(mat<R, C, T> const &lhs, mat<R, C, T> const &rhs);
        template<size_t R, size_t C, /* number */ typename T>//TODO: should be number but compiler issue is preventing that
        friend constexpr bool operator!=(mat<R, C, T> const &lhs, mat<R, C, T> const &rhs);

        constexpr vec<C, T> &operator[](size_t const index);
        constexpr vec<C, T> const &operator[](size_t const index) const;
    };
}

//Matrix aliases
namespace clove {
    using mat3f = mat<3, 3, float>;
    using mat4f = mat<4, 4, float>;
}

//Matrix operations
namespace clove {
    template<size_t R, size_t C, number T>
    constexpr mat<R, C, T> transpose(mat<R, C, T> const &m);

    template<size_t N, std::floating_point T>
    constexpr mat<N, N, T> inverse(mat<N, N, T> const &m);
}

#include "Matrix.inl"