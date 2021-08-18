#pragma once

#include "Clove/Maths/glmWrappers.hpp"

#include <glm/detail/type_mat2x2.hpp>
#include <glm/detail/type_mat3x3.hpp>
#include <glm/detail/type_mat4x4.hpp>

namespace clove {
    template<length_t C, length_t R, typename T>
    using mat = glm::mat<C, R, T, qualifier::defaultp>;

    using mat2x2f = mat<2, 2, float>;
    using mat2f   = mat2x2f;

    using mat3x3f = mat<3, 3, float>;
    using mat3f   = mat3x3f;

    using mat4x4f = mat<4, 4, float>;
    using mat4f   = mat4x4f;
}