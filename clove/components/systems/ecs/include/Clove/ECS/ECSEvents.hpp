#pragma once

#include "Clove/ECS/Entity.hpp"

namespace clove {
    template<typename ComponentType>
    struct ComponentAddedEvent {
        Entity entity;
        ComponentType &component;
    };

    template<typename ComponentType>
    struct ComponentRemovedEvent {
        Entity entity;
        ComponentType &component;
    };
}