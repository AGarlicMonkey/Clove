#pragma once

#include "Concepts.hpp"

#include <cstddef>
#include <cinttypes>

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
        friend constexpr vec<2, U> operator-(vec<2, U> const &a, vec<2, U> const &b);
        constexpr vec<2, T> &operator-=(vec<2, T> const &b);

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
        friend constexpr vec<3, U> operator-(vec<3, U> const &a, vec<3, U> const &b);
        constexpr vec<3, T> &operator-=(vec<3, T> const &b);

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
        friend constexpr vec<4, U> operator-(vec<4, U> const &a, vec<4, U> const &b);
        constexpr vec<4, T> &operator-=(vec<4, T> const &b);

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

    using vec2d = vec<2, double>;
    using vec3d = vec<3, double>;
    using vec4d = vec<4, double>;

    using vec2i = vec<2, int32_t>;
    using vec3i = vec<3, int32_t>;
    using vec4i = vec<4, int32_t>;

    using vec2u = vec<2, uint32_t>;
    using vec3u = vec<3, uint32_t>;
    using vec4u = vec<4, uint32_t>;
}

//Vector operations
namespace clove {
    /**
     * @brief Returns the length of a vector.
     * @tparam L 
     * @tparam T 
     * @param v 
     * @return 
     */
    template<size_t L, number T>
    constexpr T length(vec<L, T> const &v);

    template<size_t L, number T>
    constexpr T lengthSquared(vec<L, T> const &v);

    /**
     * @brief Returns the distance between two vectors
     * @tparam L 
     * @tparam T 
     * @param a 
     * @param b 
     * @return 
     */
    template<size_t L, number T>
    constexpr T distance(vec<L, T> const &a, vec<L, T> const &b);

    template<size_t L, number T>
    constexpr T distanceSquared(vec<L, T> const &a, vec<L, T> const &b);

    /**
     * @brief Returns a normalised version of a vector.
     * @tparam L 
     * @tparam T 
     * @param v 
     * @return 
     */
    template<size_t L, number T>
    constexpr vec<L, T> normalise(vec<L, T> const &v);

    /**
     * @brief Returns the dot product between two vectors.
     * @tparam L 
     * @tparam T 
     * @param a 
     * @param b 
     * @return 
     */
    template<size_t L, number T>
    constexpr T dot(vec<L, T> const &a, vec<L, T> const &b);

    /**
     * @brief Returns the cross product between two vectors.
     * @tparam T 
     * @param a 
     * @param b 
     * @return 
     */
    template<number T>
    constexpr vec<3, T> cross(vec<3, T> const &a, vec<3, T> const &b);

    /**
     * @brief Returns a linearly interpolated vector from a to b.
     * @details If t is 0 then the result will be equal to a. Else if t is 1 then the
     * result will be equal to b.
     * @tparam L 
     * @tparam T 
     * @param a 
     * @param b 
     * @param t A value between 0.0f and 1.0f.
     * @return 
     */
    template<size_t L, number T>
    constexpr vec<L, T> lerp(vec<L, T> const &a, vec<L, T> const &b, float const t);
}

#include "Vector.inl"