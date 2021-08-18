#pragma once

#include <Clove/ECS/Entity.hpp>

namespace clove {
    /**
     * @brief Establishes a Parent -> Child relationship to which ever entity this is attached to.
     */
    struct ParentComponent {
        Entity parent{ NullEntity };
    };
}
