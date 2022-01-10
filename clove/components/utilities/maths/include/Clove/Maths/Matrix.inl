namespace clove {
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