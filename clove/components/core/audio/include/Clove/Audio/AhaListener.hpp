#pragma once

#include <Clove/Maths/Vector.hpp>

namespace clove {
    /**
	 * @brief Handle to the listener in the 'audio scene'
	 * @details Only one listener is active when playing audio. This class
	 * exposes functions for setting values on the listener object. Such as
	 * setting the position
	 */
    class AhaListener {
        //FUNCTIONS
    public:
        virtual ~AhaListener() = default;

        virtual void setPosition(vec3f const& position) = 0;
        virtual void setVelocity(vec3f const& velocity) = 0;

        virtual vec3f getPosition() const = 0;
        virtual vec3f getVelocity() const = 0;
    };
}
