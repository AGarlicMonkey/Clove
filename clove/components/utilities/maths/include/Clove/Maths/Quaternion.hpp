#pragma once

#include "Concepts.hpp"

//Quaternion types
namespace clove {
    template<number T>
    struct quat {
        T x{ 0 };
        T y{ 0 };
        T z{ 0 };
        T w{ 1 };

        template<number U>
        friend constexpr quat<U> operator+(quat<U> const &a, quat<U> const &b);
        constexpr quat<T> &operator+=(quat<T> const &b);

        template<number U>
        friend constexpr quat<U> operator-(quat<U> const &a, quat<U> const &b);
        constexpr quat<T> &operator-=(quat<T> const &b);

        template<number U>
        friend constexpr quat<U> operator*(quat<U> const &a, U scalar);
        constexpr quat<T> &operator*=(T scalar);

        template<number U>
        friend constexpr quat<U> operator*(quat<U> const &a, quat<U> const &b);
        constexpr quat<T> &operator*=(quat<T> const &other);

        template<number U>
        friend constexpr quat<U> operator/(quat<U> const &a, U scalar);
        constexpr quat<T> &operator/=(T scalar);

        template<number U>
        friend constexpr bool operator==(quat<U> const &lhs, quat<U> const &rhs);
        template<number U>
        friend constexpr bool operator!=(quat<U> const &lhs, quat<U> const &rhs);
    };
}

//Quaternion aliases
namespace clove {
    using quatf = quat<float>;
}

//Quaternion operations
namespace clove {
    /**
     * @brief Returns the length of a quaternion.
     * @tparam T The type of the quaternion.
     * @param q 
     * @return 
     */
    template<number T>
    constexpr T length(quat<T> const &q);

    /**
     * @brief Returns a normalised quaternion.
     * @tparam T The type of the quaternion.
     * @param q 
     * @return 
     */
    template<number T>
    constexpr quat<T> normalise(quat<T> const &q);

    /**
     * @brief Returns the dot product of two quaternions.
     * @tparam T The type of each quaternion.
     * @param a 
     * @param b 
     * @return 
     */
    template<number T>
    constexpr T dot(quat<T> const &a, quat<T> const &b);

    /**
     * @brief Performs a linear interpolation between a and b by t.
     * @tparam T The type of each quaternion.
     * @param a 
     * @param b 
     * @param t A value between 0.0f and 1.0f.
     * @return 
     */
    template<number T>
    constexpr quat<T> lerp(quat<T> const &a, quat<T> const &b, float const t);

    /**
     * @brief Performs a spherical linear interpolation between a and b by t.
     * @tparam T The type of each quaternion.
     * @param a 
     * @param b 
     * @param t A value between 0.0f and 1.0f.
     * @return 
     */
    template<number T>
    constexpr quat<T> slerp(quat<T> const &a, quat<T> const &b, float const t);
}

#include "Quaternion.inl"