#pragma once

#include <cstdint>
#include <limits>

namespace blb::rnd {
    /**
	 * Variables present in Constants.glsl
	 */
    inline constexpr uint8_t MAX_LIGHTS = 10u;
    inline constexpr uint8_t MAX_JOINTS = std::numeric_limits<uint8_t>::max();

    inline constexpr uint32_t shadowMapSize = 4096u;

    /**
	 * @brief Defines a mapping between set slots
	 */
    enum class DescriptorSetSlots {
        Material = 0,
        View     = 1,
        Lighting = 2,
    };
}