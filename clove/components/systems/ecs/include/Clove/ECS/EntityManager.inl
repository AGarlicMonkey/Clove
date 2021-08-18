#include <Clove/Timer.hpp>

namespace clove {
    EventDispatcher &EntityManager::getDispatcher() {
        return ecsEventDispatcher;
    }

    template<typename ComponentType, typename... ConstructArgs>
    ComponentType &EntityManager::addComponent(Entity entity, ConstructArgs &&... args) {
        CLOVE_ASSERT(isValid(entity), "{0}: Invalid entity provided", CLOVE_FUNCTION_NAME_PRETTY);
        return componentManager.addComponent<ComponentType>(entity, std::forward<ConstructArgs>(args)...);
    }

    template<typename ComponentType>
    ComponentType &EntityManager::getComponent(Entity entity) {
        CLOVE_ASSERT(isValid(entity), "{0}: Invalid entity provided", CLOVE_FUNCTION_NAME_PRETTY);
        return componentManager.getComponent<ComponentType>(entity);
    }

    template<typename ComponentType>
    bool EntityManager::hasComponent(Entity entity) {
        if(isValid(entity)) {
            return componentManager.hasComponent<ComponentType>(entity);
        } else {
            return false;
        }
    }

    template<typename ComponentType>
    void EntityManager::removeComponent(Entity entity) {
        if(isValid(entity)) {
            componentManager.removeComponent<ComponentType>(entity);
        }
    }

    template<typename FunctionType, typename... ExcludeTypes>
    void EntityManager::forEach(FunctionType function, Exclude<ExcludeTypes...>) {
        generateViewFromFunction<FunctionType>(std::make_index_sequence<FunctionTraits<FunctionType>::arity>{}, Exclude<ExcludeTypes...>{}).forEach(function);
    }

    template<typename FunctionType, typename ClassType, typename... ExcludeTypes>
    void EntityManager::forEach(FunctionType function, ClassType *object, Exclude<ExcludeTypes...>) {
        generateViewFromFunction<FunctionType>(std::make_index_sequence<FunctionTraits<FunctionType>::arity>{}, Exclude<ExcludeTypes...>{}).forEach(function, object);
    }

    template<typename FunctionType, size_t... indices, typename... ExcludeTypes>
    auto EntityManager::generateViewFromFunction(std::index_sequence<indices...>, Exclude<ExcludeTypes...>) {
        //Expand the functions parameter tuple by using std::index_sequence to access each tuple element
        return generateView<std::tuple_element_t<indices, typename FunctionTraits<FunctionType>::DecayParameterTypesTuple>...>(Exclude<ExcludeTypes...>{});
    }

    template<typename ComponentType, typename... ComponentTypes, typename... ExcludeTypes>
    auto EntityManager::generateView(Exclude<ExcludeTypes...>) {
        //If the first template is of type Entity then pass the rest of the arguments to generateView
        if constexpr(std::is_same_v<ComponentType, Entity>) {
            return componentManager.generateView<std::decay_t<ComponentTypes>...>(Exclude<ExcludeTypes...>{});
        } else {
            return componentManager.generateView<std::decay_t<ComponentType>, std::decay_t<ComponentTypes>...>(Exclude<ExcludeTypes...>{});
        }
    }
}