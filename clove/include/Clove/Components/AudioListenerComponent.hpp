#pragma once

#include <Clove/Audio/AhaListener.hpp>
#include <memory>

namespace clove {
    /**
	 * @brief Allows an entity to determine where a sound comes from.
	 * @details Every ecs::EntityManager can only have 1 AudioListenerComponent.
	 */
    struct AudioListenerComponent {
        std::unique_ptr<AhaListener> listener;
    };
}
