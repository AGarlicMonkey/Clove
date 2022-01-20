namespace clove {
    template<size_t C, size_t R, number T>
    constexpr mat<C, R, T>::mat(T val) {
        for(size_t r{ 0 }; r < R; ++r) {
            for(size_t c{ 0 }; c < C; ++c) {
                if(r == c){
                    value[r][c] = val;
                }
            }
        }
    }

    template<size_t C, size_t R, number T>
    constexpr vec<R, T> &mat<C, R, T>::operator[](size_t const index) {
        if(index <= R) {
            return value[index];
        } else {
            return value[R - 1];
        }
    }

    template<size_t C, size_t R, number T>
    constexpr vec<R, T> const &mat<C, R, T>::operator[](size_t const index) const {
        if(index <= R) {
            return value[index];
        } else {
            return value[R - 1];
        }
    }
}