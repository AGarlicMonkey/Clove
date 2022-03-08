#pragma once

#include "Concepts.hpp"

#include <cstddef>

//Vector types
namespace clove {
    template<size_t L, number T>
    struct vec;

    template<number T>
    struct vec<2, T> {
        // clang-format off
        union { T x; T r; T u; };
        union { T y; T g; T v; };
        // clang-format on

        template<number U>
        friend constexpr vec<2, U> operator+(vec<2, U> const &a, vec<2, U> const &b);
        constexpr vec<2, T> &operator+=(vec<2, T> const &b);

        template<number U>
        friend constexpr vec<2, U> operator*(vec<2, U> const &a, U scalar);
        constexpr vec<2, T> &operator*=(T scalar);

        template<number U>
        friend constexpr vec<2, U> operator/(vec<2, U> const &a, U scalar);
        constexpr vec<2, T> &operator/=(T scalar);

        template<number U>
        friend constexpr bool operator==(vec<2, U> const &lhs, vec<2, U> const &rhs);
        template<number U>
        friend constexpr bool operator!=(vec<2, U> const &lhs, vec<2, U> const &rhs);

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };

    template<number T>
    struct vec<3, T> {
        // clang-format off
        union { T x; T r; };
        union { T y; T g; };
        union { T z; T b; };
        // clang-format on

        template<number U>
        friend constexpr vec<3, U> operator+(vec<3, U> const &a, vec<3, U> const &b);
        constexpr vec<3, T> &operator+=(vec<3, T> const &b);

        template<number U>
        friend constexpr vec<3, U> operator*(vec<3, U> const &a, U scalar);
        constexpr vec<3, T> &operator*=(T scalar);

        template<number U>
        friend constexpr vec<3, U> operator/(vec<3, U> const &a, U scalar);
        constexpr vec<3, T> &operator/=(T scalar);

        template<number U>
        friend constexpr bool operator==(vec<3, U> const &lhs, vec<3, U> const &rhs);
        template<number U>
        friend constexpr bool operator!=(vec<3, U> const &lhs, vec<3, U> const &rhs);

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };

    template<number T>
    struct vec<4, T> {
        // clang-format off
        union { T x; T r; };
        union { T y; T g; };
        union { T z; T b; };
        union { T w; T a; };
        // clang-format on

        template<number U>
        friend constexpr vec<4, U> operator+(vec<4, U> const &a, vec<4, U> const &b);
        constexpr vec<4, T> &operator+=(vec<4, T> const &b);

        template<number U>
        friend constexpr vec<4, U> operator*(vec<4, U> const &a, U scalar);
        constexpr vec<4, T> &operator*=(T scalar);

        template<number U>
        friend constexpr vec<4, U> operator/(vec<4, U> const &a, U scalar);
        constexpr vec<4, T> &operator/=(T scalar);

        template<number U>
        friend constexpr bool operator==(vec<4, U> const &lhs, vec<4, U> const &rhs);
        template<number U>
        friend constexpr bool operator!=(vec<4, U> const &lhs, vec<4, U> const &rhs);

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };
}

//Vector aliases
namespace clove {
    using vec2f = vec<2, float>;
    using vec3f = vec<3, float>;
    using vec4f = vec<4, float>;
}

//Vector operations
namespace clove {
    template<size_t L, number T>
    constexpr T length(vec<L, T> const &v);

    template<size_t L, number T>
    constexpr vec<L, T> normalise(vec<L, T> const &v);

    template<size_t L, number T>
    constexpr T dot(vec<L, T> const &a, vec<L, T> const &b);

    template<number T>
    constexpr vec<3, T> cross(vec<3, T> const &a, vec<3, T> const &b);
}

#include "Vector.inl"