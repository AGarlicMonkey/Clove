#pragma once

#include <concepts>

//Vector types
namespace clove {
    template<typename T>
    concept number = std::integral<T> || std::floating_point<T>;

    template<size_t length, number T>
    struct vec;

    template<number T>
    struct vec<1, T> {
        union {
            T x;
            T r;
            T u;
        };


        template<number T>
        friend constexpr bool operator==(vec<1, T> const &lhs, vec<1, T> const &rhs);
        template<number T>
        friend constexpr bool operator!=(vec<1, T> const &lhs, vec<1, T> const &rhs);

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };

    template<number T>
    struct vec<2, T> {
        union {
            T x;
            T r;
            T u;
        };
        union {
            T y;
            T g;
            T v;
        };


        template<number T>
        friend constexpr bool operator==(vec<2, T> const &lhs, vec<2, T> const &rhs);
        template<number T>
        friend constexpr bool operator!=(vec<2, T> const &lhs, vec<2, T> const &rhs);

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };

    template<number T>
    struct vec<3, T> {
        union {
            T x;
            T r;
        };
        union {
            T y;
            T g;
        };
        union {
            T z;
            T b;
        };


        template<number T>
        friend constexpr bool operator==(vec<3, T> const &lhs, vec<3, T> const &rhs);
        template<number T>
        friend constexpr bool operator!=(vec<3, T> const &lhs, vec<3, T> const &rhs);

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };

    template<number T>
    struct vec<4, T> {
        union {
            T x;
            T r;
        };
        union {
            T y;
            T g;
        };
        union {
            T z;
            T b;
        };
        union {
            T w;
            T a;
        };


        template<number T>
        friend constexpr bool operator==(vec<4, T> const &lhs, vec<4, T> const &rhs);
        template<number T>
        friend constexpr bool operator!=(vec<4, T> const &lhs, vec<4, T> const &rhs);

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };

    using vec1f = vec<1, float>;
    using vec2f = vec<2, float>;
    using vec3f = vec<3, float>;
    using vec4f = vec<4, float>;
}

//Vector operations
namespace clove {
    template<size_t L, typename T>
    constexpr T length(vec<L, T> const &vec);
}

#include "Vector.inl"