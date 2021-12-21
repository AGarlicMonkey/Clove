#include <cmath>

namespace clove {
    template<number T>
    constexpr vec<1, T> operator+(vec<1, T> const &a, vec<1, T> const &b) {
        return { a.x + b.x };
    }

    template<number T>
    constexpr vec<1, T> &vec<1, T>::operator+=(vec<1, T> const &b) {
        x += b.x;

        return *this;
    }

    template<number T>
    constexpr bool operator==(vec<1, T> const &lhs, vec<1, T> const &rhs) {
        return lhs.x == rhs.x;
    }

    template<number T>
    constexpr bool operator!=(vec<1, T> const &lhs, vec<1, T> const &rhs) {
        return !(lhs == rhs);
    }

    template<number T>
    constexpr T &vec<1, T>::operator[](size_t const index) {
        return x;
    }

    template<number T>
    constexpr T const &vec<1, T>::operator[](size_t const index) const {
        return x;
    }

    template<number T>
    constexpr vec<2, T> operator+(vec<2, T> const &a, vec<2, T> const &b) {
        return { a.x + b.x, a.y + b.y };
    }

    template<number T>
    constexpr vec<2, T> &vec<2, T>::operator+=(vec<2, T> const &b) {
        x += b.x;
        y += b.y;

        return *this;
    }

    template<number T>
    constexpr bool operator==(vec<2, T> const &lhs, vec<2, T> const &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    template<number T>
    constexpr bool operator!=(vec<2, T> const &lhs, vec<2, T> const &rhs) {
        return !(lhs == rhs);
    }

    template<number T>
    constexpr T &vec<2, T>::operator[](size_t const index) {
        if(index <= 1) {
            T *mem{ &x };
            return mem[index];
        } else {
            return y;
        }
    }

    template<number T>
    constexpr T const &vec<2, T>::operator[](size_t const index) const {
        if(index <= 1) {
            T const *mem{ &x };
            return mem[index];
        } else {
            return y;
        }
    }

    template<number T>
    constexpr vec<3, T> operator+(vec<3, T> const &a, vec<3, T> const &b) {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    }

    template<number T>
    constexpr vec<3, T> &vec<3, T>::operator+=(vec<3, T> const &b) {
        x += b.x;
        y += b.y;
        z += b.z;

        return *this;
    }

    template<number T>
    constexpr bool operator==(vec<3, T> const &lhs, vec<3, T> const &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    template<number T>
    constexpr bool operator!=(vec<3, T> const &lhs, vec<3, T> const &rhs) {
        return !(lhs == rhs);
    }

    template<number T>
    constexpr T &vec<3, T>::operator[](size_t const index) {
        if(index <= 2) {
            T *mem{ &x };
            return mem[index];
        } else {
            return z;
        }
    }

    template<number T>
    constexpr T const &vec<3, T>::operator[](size_t const index) const {
        if(index <= 2) {
            T const *mem{ &x };
            return mem[index];
        } else {
            return z;
        }
    }

    template<number T>
    constexpr vec<4, T> operator+(vec<4, T> const &a, vec<4, T> const &b) {
        return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    }

    template<number T>
    constexpr vec<4, T> &vec<4, T>::operator+=(vec<4, T> const &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        w += b.w;

        return *this;
    }

    template<number T>
    constexpr bool operator==(vec<4, T> const &lhs, vec<4, T> const &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }

    template<number T>
    constexpr bool operator!=(vec<4, T> const &lhs, vec<4, T> const &rhs) {
        return !(lhs == rhs);
    }

    template<number T>
    constexpr T &vec<4, T>::operator[](size_t const index) {
        if(index <= 3) {
            T *mem{ &x };
            return mem[index];
        } else {
            return w;
        }
    }

    template<number T>
    constexpr T const &vec<4, T>::operator[](size_t const index) const {
        if(index <= 3) {
            T const *mem{ &x };
            return mem[index];
        } else {
            return w;
        }
    }

    template<size_t L, number T>
    constexpr T length(vec<L, T> const &vec) {
        T sum{ 0 };
        for(size_t i{ 0 }; i < L; ++i) {
            sum += vec[i];
        }
        return std::sqrt(sum);
    }

    template<size_t L, number T>
    constexpr vec<L, T> normalise(vec<L, T> const &v) {
        // return v / length(v);
        return {};
    }
}