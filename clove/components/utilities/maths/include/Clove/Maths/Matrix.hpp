#pragma once

#include "Concepts.hpp"
#include "Vector.hpp"

#include <array>

//Matrix types
namespace clove {
    template<size_t R, size_t C, number T>
    struct mat {
        std::array<vec<C, T>, R> rows{};
    };
}

//Matrix aliases
namespace clove {
    using mat4f = mat<4, 4, float>;
}