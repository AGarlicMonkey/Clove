#include <cmath>

namespace clove {
    template<number T>
    constexpr T length(quat<T> const &q) {
        return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    }

}