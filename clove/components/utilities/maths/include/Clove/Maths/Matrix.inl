namespace clove {
    template<size_t R, size_t C, number T>
    mat<R, C, T>::mat(T val) {
        for(size_t r{ 0 }; r < R; ++r) {
            for(size_t c{ 0 }; c < C; ++c) {
                if(r == c){
                    rows[r][c] = val;
                }
            }
        }
    }

    template<size_t R, size_t C, number T>
    constexpr vec<C, T> &mat<R, C, T>::operator[](size_t const index) {
        if(index <= R) {
            return rows[index];
        } else {
            return rows[R - 1];
        }
    }

    template<size_t R, size_t C, number T>
    constexpr vec<C, T> const &mat<R, C, T>::operator[](size_t const index) const {
        if(index <= R) {
            return rows[index];
        } else {
            return rows[R - 1];
        }
    }
}