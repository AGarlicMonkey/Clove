#include <cmath>

namespace clove {
    template<number T>
    constexpr T &vec<1, T>::operator[](size_t const index) {
        return x;
    }

    template<number T>
    constexpr T const &vec<1, T>::operator[](size_t const index) const {
        return x;
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
        return v / length(v);
    }
}