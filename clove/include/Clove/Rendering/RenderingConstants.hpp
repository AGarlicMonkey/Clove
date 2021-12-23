#pragma once

#include "Clove/Rendering/AnimationTypes.hpp"

#include <cstdint>
#include <limits>

namespace clove {
	//Variables present in Constants.glsl
    inline uint8_t constexpr MAX_LIGHTS{ 10u };
    inline uint8_t constexpr MAX_JOINTS{ std::numeric_limits<JointIndexType>::max() };

    //General constants
    inline uint32_t constexpr shadowMapSize{ 1024u };
    inline size_t constexpr cubeMapLayerCount{ 6 };
}