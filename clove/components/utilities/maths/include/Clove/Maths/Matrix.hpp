#pragma once

#include "Concepts.hpp"
#include "Vector.hpp"

#include <array>

//Matrix types
namespace clove {
    template<size_t C, size_t R, number T>
    struct mat {
        std::array<vec<R, T>, C> value{};

        mat() = default;
        mat(T val);

        constexpr vec<R, T> &operator[](size_t const index);
        constexpr vec<R, T> const &operator[](size_t const index) const;
    };
}

//Matrix aliases
namespace clove {
    using mat4f = mat<4, 4, float>;
}

#include "Matrix.inl"