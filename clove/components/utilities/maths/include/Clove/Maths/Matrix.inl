namespace clove {
    template<size_t R, size_t C, number T>
    constexpr mat<R, C, T>::mat(T val) {
        for(size_t r{ 0 }; r < R; ++r) {
            for(size_t c{ 0 }; c < C; ++c) {
                if(r == c) {
                    value[r][c] = val;
                }
            }
        }
    }

    template<size_t R, size_t C, number T>
    constexpr vec<C, T> operator*(mat<R, C, T> const &m, vec<C, T> const &v) {
        vec<C, T> result{};

        for(size_t i{ 0 }; i < C; ++i) {
            result[i] = dot(m[i], v);
        }

        return result;
    }

    template<size_t R1, size_t R2, size_t C, number T>
    constexpr mat<R1, R2, T> operator*(mat<R1, C, T> const &a, mat<C, R2, T> const &b) {
        mat<R1, R2, T> result{};

        for(size_t r1{ 0 }; r1 < R1; ++r1) {
            for(size_t r2{ 0 }; r2 < R2; ++r2) {
                for(size_t c{ 0 }; c < C; ++c){
                    result[r1][r2] += a[r1][c] * b[c][r2];
                }
            }
        }

        return result;
    }

    template<size_t R, size_t C, number T>
    constexpr vec<C, T> &mat<R, C, T>::operator[](size_t const index) {
        if(index <= R) {
            return value[index];
        } else {
            return value[R - 1];
        }
    }

    template<size_t R, size_t C, number T>
    constexpr vec<C, T> const &mat<R, C, T>::operator[](size_t const index) const {
        if(index <= R) {
            return value[index];
        } else {
            return value[R - 1];
        }
    }

    template<size_t R, size_t C, number T>
    constexpr mat<R, C, T> transpose(mat<R, C, T> const &m) {
        mat<R, C, T> transposed{};

        for(size_t r{ 0 }; r < R; ++r) {
            for(size_t c{ 0 }; c < C; ++c) {
                transposed[r][c] = m[c][r];
            }
        }

        return transposed;
    }
}