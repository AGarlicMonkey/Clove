#pragma once

//Wrapping glm in clv::mth

#include "Core/Maths/glmWrappers.hpp"

#include <glm/glm.hpp>

namespace clv::mth{
	template<length_t L, typename T, qualifier Q>
	using vec = glm::vec<L, T, Q>;

	using vec1i = vec<1, int32, qualifier::defaultp>;
	using vec2i = vec<2, int32, qualifier::defaultp>;
	using vec3i = vec<3, int32, qualifier::defaultp>;
	using vec4i = vec<4, int32, qualifier::defaultp>;

	using vec1f = vec<1, float, qualifier::defaultp>;
	using vec2f = vec<2, float, qualifier::defaultp>;
	using vec3f = vec<3, float, qualifier::defaultp>;
	using vec4f = vec<4, float, qualifier::defaultp>;

	//TODO: All the other ps
}