#pragma once

#include "Clove/Maths/glmWrappers.hpp"

#include <glm/detail/type_vec1.hpp>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>

namespace clove {
    template<length_t L, typename T>
    using vec = glm::vec<L, T, qualifier::defaultp>;

    using vec1i = vec<1, int32_t>;
    using vec2i = vec<2, int32_t>;
    using vec3i = vec<3, int32_t>;
    using vec4i = vec<4, int32_t>;

    using vec1ui = vec<1, uint32_t>;
    using vec2ui = vec<2, uint32_t>;
    using vec3ui = vec<3, uint32_t>;
    using vec4ui = vec<4, uint32_t>;

    using vec1f = vec<1, float>;
    using vec2f = vec<2, float>;
    using vec3f = vec<3, float>;
    using vec4f = vec<4, float>;
}