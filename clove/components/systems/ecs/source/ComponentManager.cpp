#include "Clove/ECS/ComponentManager.hpp"

namespace clove {
    ComponentManager::ComponentManager(EventDispatcher *dispatcher)
        : ecsEventDispatcher(dispatcher) {
    }

    ComponentManager::~ComponentManager() = default;

    void ComponentManager::cloneComponents(Entity from, Entity to) {
        for(auto &[key, container] : containers) {
            if(container->hasComponent(from)) {
                container->cloneComponent(from, to);
            }
        }
    }

    void ComponentManager::removeEntity(Entity entity) {
        for(auto &[key, container] : containers) {
            if(container->hasComponent(entity)) {
                container->removeComponent(entity);
            }
        }
    }
}
