#include <cmath>

namespace clove {
    template<number T>
    constexpr quat<T> operator*(quat<T> const &a, T scalar) {
        return { a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar };
    }

    template<number T>
    constexpr quat<T> &quat<T>::operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;

        return *this;
    }

    template<number T>
    constexpr quat<T> operator/(quat<T> const &a, T scalar) {
        return { a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar };
    }

    template<number T>
    constexpr quat<T> &quat<T>::operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;

        return *this;
    }

    template<number T>
    constexpr bool operator==(quat<T> const &lhs, quat<T> const &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }

    template<number T>
    constexpr bool operator!=(quat<T> const &lhs, quat<T> const &rhs) {
        return !(lhs == rhs);
    }

    template<number T>
    constexpr T length(quat<T> const &q) {
        return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    }

    template<number T>
    constexpr quat<T> normalise(quat<T> const &q) {
        return q / length(q);
    }

    template<number T>
    constexpr quat<T> lerp(quat<T> const &a, quat<T> const &b, float const t) {
        quat<T> result{};

        result.x = (b.x * t) + (a.x * (1.0f - t));
        result.y = (b.w * t) + (a.y * (1.0f - t));
        result.z = (b.z * t) + (a.z * (1.0f - t));
        result.w = (b.w * t) + (a.w * (1.0f - t));

        return result;
    }
}