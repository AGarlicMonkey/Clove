#pragma once

#include "Clove/ECS/ComponentContainer.hpp"
#include "Clove/ECS/ComponentView.hpp"
#include "Clove/ECS/Entity.hpp"

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace clove {
    class EventDispatcher;
}

namespace clove {
    class ComponentManager {
        //VARIABLES
    private:
        std::unordered_map<std::type_index, std::unique_ptr<ComponentContainerInterface>> containers{};

        EventDispatcher *ecsEventDispatcher{ nullptr };

        //FUNCTIONS
    public:
        ComponentManager() = delete;
        ComponentManager(EventDispatcher *dispatcher);

        ComponentManager(ComponentManager const &other)     = delete;
        ComponentManager(ComponentManager &&other) noexcept = delete;

        ComponentManager &operator=(ComponentManager const &other) = delete;
        ComponentManager &operator=(ComponentManager &&other) noexcept = delete;

        ~ComponentManager();

        template<typename ComponentType, typename... ConstructArgs>
        ComponentType &addComponent(Entity entity, ConstructArgs &&... args);
        template<typename ComponentType>
        ComponentType &getComponent(Entity entity);

        template<typename ComponentType>
        bool hasComponent(Entity entity);
        void cloneComponents(Entity from, Entity to);
        template<typename ComponentType>
        void removeComponent(Entity entity);

        void removeEntity(Entity entity);

        template<typename... ComponentTypes, typename... ExcludeTypes>
        ComponentView<Exclude<ExcludeTypes...>, ComponentTypes...> generateView(Exclude<ExcludeTypes...>);

    private:
        template<typename ComponentType>
        ComponentContainer<ComponentType> &getContainer();
    };
}

#include "ComponentManager.inl"