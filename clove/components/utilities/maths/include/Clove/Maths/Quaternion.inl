#include <cmath>

namespace clove {
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

}