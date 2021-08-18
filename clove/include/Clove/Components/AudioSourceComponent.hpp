#pragma once

#include <Clove/Audio/AhaSource.hpp>
#include <memory>

namespace clove {
    /**
	 * @brief Allows an entity to determine where a sound is coming from.
	 */
    struct AudioSourceComponent {
        std::unique_ptr<AhaSource> source;
    };
}
