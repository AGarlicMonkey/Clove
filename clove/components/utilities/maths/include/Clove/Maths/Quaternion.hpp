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
        friend constexpr quat<U> operator*(quat<U> const &a, U scalar);
        constexpr quat<T> &operator*=(T scalar);

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
    template<number T>
    constexpr T length(quat<T> const &q);

}

#include "Quaternion.inl"