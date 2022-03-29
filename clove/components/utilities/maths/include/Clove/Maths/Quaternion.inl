#include "Clove/Maths/MathsHelpers.hpp"

#include <cmath>

namespace clove {
    template<number T>
    constexpr quat<T> operator+(quat<T> const &a, quat<T> const &b) {
        return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    }

    template<number T>
    constexpr quat<T> &quat<T>::operator+=(quat<T> const &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        w += b.w;

        return *this;
    }

    template<number T>
    constexpr quat<T> operator-(quat<T> const &a, quat<T> const &b) {
        return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    }

    template<number T>
    constexpr quat<T> &quat<T>::operator-=(quat<T> const &b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        w -= b.w;

        return *this;
    }

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
    constexpr quat<T> operator*(quat<T> const &a, quat<T> const &b) {
        return quat<T>{ a } *= b;
    }

    template<number T>
    constexpr quat<T> &quat<T>::operator*=(quat<T> const &other) {
        quat<T> const p{ *this };
        quat<T> const q{ other };

        x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
        y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
        z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
        w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;

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
        return areFloatsEqual(lhs.x, rhs.x) && areFloatsEqual(lhs.y, rhs.y) && areFloatsEqual(lhs.z, rhs.z) && areFloatsEqual(lhs.w, rhs.w);
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
    constexpr T dot(quat<T> const &a, quat<T> const &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
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

    template<number T>
    constexpr quat<T> slerp(quat<T> const &a, quat<T> const &b, float const t) {
        T const angle{ std::acos(dot(a, b)) };
        T const denom{ std::sin(angle) };

        return ((b * std::sin(t * angle)) + (a * std::sin((1.0f - t) * angle))) / denom;
    }
}