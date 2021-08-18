#pragma once

#include <Clove/Maths/Vector.hpp>

namespace clove {
	class DrawableElement {
		//FUNCTIONS
	public:
		virtual ~DrawableElement() = default;

        virtual void draw(const vec2f &drawSpace) = 0;
    };
}