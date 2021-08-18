#include "Clove/ECS/ECSEvents.hpp"

#include <Clove/Event/EventDispatcher.hpp>
#include <typeinfo>

namespace clove {
    template<typename ComponentType, typename... ConstructArgs>
    ComponentType &ComponentManager::addComponent(Entity entity, ConstructArgs &&... args) {
        return getContainer<ComponentType>().addComponent(entity, std::forward<ConstructArgs>(args)...);
    }

    template<typename ComponentType>
    ComponentType &ComponentManager::getComponent(Entity entity) {
        return getContainer<ComponentType>().getComponent(entity);
    }

    template<typename ComponentType>
    bool ComponentManager::hasComponent(Entity entity) {
        return getContainer<ComponentType>().hasComponent(entity);
    }

    template<typename ComponentType>
    void ComponentManager::removeComponent(Entity entity) {
        getContainer<ComponentType>().removeComponent(entity);
    }

    template<typename... ComponentTypes, typename... ExcludeTypes>
    ComponentView<Exclude<ExcludeTypes...>, ComponentTypes...> ComponentManager::generateView(Exclude<ExcludeTypes...>) {
        if constexpr(sizeof...(ExcludeTypes) > 0) {
            return { std::make_tuple(&getContainer<ComponentTypes>()...), std::make_tuple(&getContainer<ExcludeTypes>()...) };
        } else if constexpr(sizeof...(ComponentTypes) > 1) {
            return { std::make_tuple(&getContainer<ComponentTypes>()...) };
        } else {
            return { &getContainer<ComponentTypes>()... };
        }
    }

    template<typename ComponentType>
    ComponentContainer<ComponentType> &ComponentManager::getContainer() {
        std::type_index const componentIndex{ typeid(ComponentType) };

        if(auto iter{ containers.find(componentIndex) }; iter != containers.end()) {
            return static_cast<ComponentContainer<ComponentType> &>(*iter->second.get());
        } else {
            containers[componentIndex] = std::make_unique<ComponentContainer<ComponentType>>(ecsEventDispatcher);
            return static_cast<ComponentContainer<ComponentType> &>(*containers[componentIndex].get());
        }
    }
}