#include "Clove/ECS/ECSEvents.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Event/EventDispatcher.hpp>
#include <Clove/Log/Log.hpp>
#include <type_traits>

namespace clove {
    bool ComponentContainerInterface::hasComponent(Entity entity) {
        return entity != NullEntity && entityToIndex.size() > entity && entityToIndex[entity] != nullIndex;
    }

    ComponentContainerInterface::IndexType ComponentContainerInterface::size() const {
        return entities.size();
    }

    ComponentContainerInterface::Iterator ComponentContainerInterface::begin() noexcept {
        return entities.begin();
    }

    ComponentContainerInterface::Iterator ComponentContainerInterface::end() noexcept {
        return entities.end();
    }

    template<typename ComponentType>
    ComponentContainer<ComponentType>::ComponentContainer(EventDispatcher *dispatcher)
        : ecsEventDispatcher{ dispatcher } {
    }

    template<typename ComponentType>
    ComponentContainer<ComponentType>::ComponentContainer(ComponentContainer &&other) noexcept = default;

    template<typename ComponentType>
    ComponentContainer<ComponentType> &ComponentContainer<ComponentType>::operator=(ComponentContainer &&other) noexcept = default;

    template<typename ComponentType>
    ComponentContainer<ComponentType>::~ComponentContainer() = default;

    template<typename ComponentType>
    template<typename... ConstructArgs>
    ComponentType &ComponentContainer<ComponentType>::addComponent(Entity entity, ConstructArgs &&... args) {
        if(!hasComponent(entity)) {
            if(entityToIndex.size() <= entity) {
                entityToIndex.resize(entity + 1, nullIndex);
            }

            entities.push_back(entity);
            components.emplace_back(ComponentType{ std::forward<ConstructArgs>(args)... });

            entityToIndex[entity] = entities.size() - 1;
        } else {
            components[entityToIndex[entity]] = ComponentType{ std::forward<ConstructArgs>(args)... };
        }

        ecsEventDispatcher->broadCastEvent(ComponentAddedEvent<ComponentType>{ entity, components[entityToIndex[entity]] });

        return components[entityToIndex[entity]];
    }

    template<typename ComponentType>
    ComponentType &ComponentContainer<ComponentType>::getComponent(Entity entity) {
        CLOVE_ASSERT(hasComponent(entity), "{0}: Entity does not have component", CLOVE_FUNCTION_NAME_PRETTY);
        return components[entityToIndex[entity]];
    }

    template<typename ComponentType>
    void ComponentContainer<ComponentType>::cloneComponent(Entity from, Entity to) {
        if constexpr(std::is_copy_constructible_v<ComponentType>) {
            addComponent(to, getComponent(from));
        } else {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Warning, "{0}: Component is not copy constructable. Entity {1} will be incomplete.", CLOVE_FUNCTION_NAME_PRETTY, to);
        }
    }

    template<typename ComponentType>
    void ComponentContainer<ComponentType>::removeComponent(Entity entity) {
        if(hasComponent(entity)) {
            ecsEventDispatcher->broadCastEvent(ComponentRemovedEvent<ComponentType>{ entity, components[entityToIndex[entity]] });

            IndexType const index{ entityToIndex[entity] };
            IndexType const lastIndex{ entities.size() - 1 };

            if(index < lastIndex) {
                Entity const movedEntity{ entities[lastIndex] };

                entities[index]   = std::move(entities.back());
                components[index] = std::move(components.back());

                entityToIndex[movedEntity] = index;
            }

            entities.pop_back();
            components.pop_back();

            entityToIndex[entity] = nullIndex;
        }
    }
}