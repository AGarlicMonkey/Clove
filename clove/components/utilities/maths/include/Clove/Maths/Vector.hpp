#pragma once

#include <concepts>

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

        constexpr T &operator[](size_t const index);
        constexpr T const &operator[](size_t const index) const;
    };

    using vec1f = vec<1, float>;
    using vec2f = vec<2, float>;
    using vec3f = vec<3, float>;
    using vec4f = vec<4, float>;
}

namespace clove {
    template<size_t L, typename T>
    constexpr T length(vec<L, T> const &vec);
}

#include "Vector.inl"