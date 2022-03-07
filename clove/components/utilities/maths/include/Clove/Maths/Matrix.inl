namespace clove {
    namespace internal {
        template<size_t size>
        struct SizeToIntType;

        template<>
        struct SizeToIntType<4> {
            using intType  = int32_t;
            using uintType = uint32_t;
        };

        template<>
        struct SizeToIntType<8> {
            using intType  = int64_t;
            using uintType = uint64_t;
        };

        template<typename T>
        constexpr bool compareFloatingPoint(T a, T b) requires std::is_floating_point_v<T> {
            using intType = SizeToIntType<sizeof(T)>::intType;

            union fpUnion {
                T val;
                intType bits;
            };

            size_t constexpr numBits{ 8 * sizeof(T) };
            intType constexpr signMask{ static_cast<intType>(1) << (numBits - 1) };

            intType constexpr maxUlps{ 4 };

            fpUnion const aUnion{ a };
            fpUnion const bUnion{ b };

            intType const aInt{ signMask & aUnion.bits ? ~aUnion.bits + 1 : signMask | aUnion.bits };
            intType const bInt{ signMask & bUnion.bits ? ~bUnion.bits + 1 : signMask | bUnion.bits };

            intType const intDiff{ aInt >= bInt ? aInt - bInt : bInt - aInt };
            return intDiff <= maxUlps;
        }

        template<size_t N, std::floating_point T>
        mat<N, N, T> getCofactor(mat<N, N, T> const &m, int32_t p, int32_t q, int32_t n) {
            int32_t i{ 0 };
            int32_t j{ 0 };
            mat<N, N, T> result{};

            for(int32_t r{ 0 }; r < n; ++r) {
                for(int32_t c{ 0 }; c < n; ++c) {
                    if(r != p && c != q) {
                        result[i][j++] = m[r][c];
                        if(j == n - 1) {
                            j = 0;
                            i++;
                        }
                    }
                }
            }

            return result;
        }

        template<size_t N, std::floating_point T>
        T getDeterminant(mat<N, N, T> const &m, int32_t n) {
            T determinant{ 0 };

            if(n != 1) {
                int32_t sign{ 1 };
                for(int32_t f{ 0 }; f < n; ++f) {
                    mat<N, N, T> const cofactors{ getCofactor(m, 0, f, n) };
                    determinant += sign * m[0][f] * getDeterminant(cofactors, n - 1);
                    sign = -sign;
                }
            } else {
                determinant = m[0][0];
            }

            return determinant;
        }

        template<size_t N, std::floating_point T>
        mat<N, N, T> getAdjoint(mat<N, N, T> const &m) {
            mat<N, N, T> adj{};

            if(N != 1) {
                int32_t sign{ 1 };
                for(int32_t i{ 0 }; i < N; ++i) {
                    for(int32_t j{ 0 }; j < N; ++j) {
                        mat<N, N, T> const cofactors{ getCofactor(m, i, j, N) };
                        sign      = ((i + j) % 2 == 0) ? 1 : -1;
                        adj[j][i] = sign * (getDeterminant(cofactors, N - 1));//Interchange rows and columns to get the transpose of the cofactor matrix
                    }
                }
            } else {
                adj[0][0] = 1;
            }

            return adj;
        }
    }

    template<size_t R, size_t C, number T>
    constexpr mat<R, C, T>::mat(T val) {
        for(size_t r{ 0 }; r < R; ++r) {
            for(size_t c{ 0 }; c < C; ++c) {
                if(r == c) {
                    value[r][c] = val;
                } else {
                    value[r][c] = T{};
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
                for(size_t c{ 0 }; c < C; ++c) {
                    result[r1][r2] += a[r1][c] * b[c][r2];
                }
            }
        }

        return result;
    }

    template<size_t R, size_t C, number T>
    constexpr bool operator==(mat<R, C, T> const &lhs, mat<R, C, T> const &rhs) {
        for(size_t r{ 0 }; r < R; ++r) {
            for(size_t c{ 0 }; c < C; ++c) {
                if constexpr(std::is_floating_point_v<T>) {
                    if(!internal::compareFloatingPoint(lhs[r][c], rhs[r][c])) {
                        return false;
                    }
                } else {
                    if(lhs[r][c] != rhs[r][c]) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    template<size_t R, size_t C, number T>
    constexpr bool operator!=(mat<R, C, T> const &lhs, mat<R, C, T> const &rhs) {
        return !(lhs == rhs);
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

    template<size_t N, std::floating_point T>
    constexpr mat<N, N, T> inverse(mat<N, N, T> const &m) {
        T const determinant{ internal::getDeterminant(m, N) };
        mat<N, N, T> const adj{ internal::getAdjoint(m) };

        mat<N, N, T> result;
        for(int i{ 0 }; i < N; ++i) {
            for(int j{ 0 }; j < N; ++j) {
                result[i][j] = adj[i][j] / determinant;
            }
        }

        return result;
    }
}