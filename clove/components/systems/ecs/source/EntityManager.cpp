#include "Clove/ECS/EntityManager.hpp"

namespace clove {
    Entity EntityManager::nextEntity{ 1 };

    EntityManager::EntityManager()
        : componentManager(&ecsEventDispatcher) {
    }

    EntityManager::~EntityManager() {
        destroyAll();
    }

    Entity EntityManager::create() {
        Entity entity{ nextEntity++ };
        activeEntities.push_back(entity);

        return entity;
    }

    Entity EntityManager::clone(Entity entity) {
        if(!isValid(entity)){
            return NullEntity;
        }

        Entity clonedEntity{ create() };
        componentManager.cloneComponents(entity, clonedEntity);

        return clonedEntity;
    }

    void EntityManager::destroy(Entity entity) {
        if(!isValid(entity)) {
            return;
        }

        std::erase(activeEntities, entity);
        componentManager.removeEntity(entity);
    }

    void EntityManager::destroyAll() {
        for(Entity entity : activeEntities) {
            componentManager.removeEntity(entity);
        }
        activeEntities.clear();
    }

    bool EntityManager::isValid(Entity entity) {
        if(entity != NullEntity) {
            return std::find(activeEntities.begin(), activeEntities.end(), entity) != activeEntities.end();
        } else {
            return false;
        }
    }
}
