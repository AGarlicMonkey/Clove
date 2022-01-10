#pragma once

#include "Concepts.hpp"
#include "Vector.hpp"

#include <array>

//Matrix types
namespace clove {
    template<size_t rows, size_t columns, number T>
    struct mat {
        std::array<vec<columns, T>, rows> rows{};
    };
}

//Matrix aliases
namespace clove {
    using mat4f = mat<4, 4, float>;
}