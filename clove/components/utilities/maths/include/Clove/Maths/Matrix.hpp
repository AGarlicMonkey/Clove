#pragma once

#include "Concepts.hpp"
#include "Vector.hpp"

#include <array>
#include <cstddef>

//Matrix types
namespace clove {
    template<size_t R, size_t C, number T>
    struct mat {
        std::array<vec<C, T>, R> value{};

        constexpr mat() = default;
        constexpr mat(T val);

        template<size_t R1, size_t C1, number U>
        friend constexpr vec<C1, U> operator*(mat<R1, C1, U> const &m, vec<C1, U> const &v);

        template<size_t R1, size_t R2, size_t C1, number U>
        friend constexpr mat<R1, R2, U> operator*(mat<R1, C1, U> const &a, mat<C1, R2, U> const &b);

        template<size_t R1, size_t C1, number U>
        friend constexpr bool operator==(mat<R1, C1, U> const &lhs, mat<R1, C1, U> const &rhs);
        template<size_t R1, size_t C1, number U>
        friend constexpr bool operator!=(mat<R1, C1, U> const &lhs, mat<R1, C1, U> const &rhs);

        constexpr vec<C, T> &operator[](size_t const index);
        constexpr vec<C, T> const &operator[](size_t const index) const;
    };
}

//Matrix aliases
namespace clove {
    using mat3f = mat<3, 3, float>;
    using mat4f = mat<4, 4, float>;
}

//Matrix operations
namespace clove {
    template<size_t R, size_t C, number T>
    constexpr mat<R, C, T> transpose(mat<R, C, T> const &m);

    template<size_t N, std::floating_point T>
    constexpr mat<N, N, T> inverse(mat<N, N, T> const &m);
}

#include "Matrix.inl"