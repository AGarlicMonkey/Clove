#pragma once

#include "Concepts.hpp"
#include "Vector.hpp"

#include <array>

//Matrix types
namespace clove {
    template<size_t R, size_t C, number T> 
    struct mat {
        std::array<vec<C, T>, R> value{};

        constexpr mat() = default;
        constexpr mat(T val);

        template<size_t R, size_t C, /* number */ typename T> //TODO: should be number but compiler issue is preventing that
        friend constexpr vec<C, T> operator*(mat<R, C, T> const &a, vec<C, T> const &b);

        constexpr vec<C, T> &operator[](size_t const index);
        constexpr vec<C, T> const &operator[](size_t const index) const;
    };
}

//Matrix aliases
namespace clove {
    using mat4f = mat<4, 4, float>;
}

//Matrix operations
namespace clove {
    template<size_t R, size_t C, number T>
    constexpr mat<R, C, T> transpose(mat<R, C, T> const &m);

}

#include "Matrix.inl"