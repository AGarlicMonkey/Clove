namespace clove {
    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::ComponentView(std::tuple<ComponentContainer<ComponentTypes> *...> containerViews)
        : containerViews{ std::move(containerViews) }
        , drivingContainer{ chooseDrivingContainer() } {
    }

    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::ComponentView(std::tuple<ComponentContainer<ComponentTypes> *...> containerViews, std::tuple<ComponentContainer<ExcludedTypes> *...> excludedContainers)
        : containerViews{ std::move(containerViews) }
        , excludedContainers{ std::move(excludedContainers) }
        , drivingContainer{ chooseDrivingContainer() } {
    }

    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::ComponentView(ComponentView const &other) = default;

    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::ComponentView(ComponentView &&other) noexcept = default;

    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...> &ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::operator=(ComponentView const &other) = default;

    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...> &ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::operator=(ComponentView &&other) noexcept = default;

    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::~ComponentView() = default;

    template<typename... ExcludedTypes, typename... ComponentTypes>
    template<typename CallableType>
    void ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::forEach(CallableType callable) {
        for(auto entity : *drivingContainer) {
            if constexpr(sizeof...(ExcludedTypes) > 0) {
                if((std::get<ComponentContainer<ExcludedTypes> *>(excludedContainers)->hasComponent(entity) || ...)) {
                    continue;
                }
            }

            if((std::get<ComponentContainer<ComponentTypes> *>(containerViews)->hasComponent(entity) && ...)) {
                //Call with entity if it's the first argument
                if constexpr(std::is_same_v<Entity, std::tuple_element_t<0, typename FunctionTraits<CallableType>::ParameterTypesTuple>>) {
                    callable(entity, std::get<ComponentContainer<ComponentTypes> *>(containerViews)->getComponent(entity)...);
                } else {
                    callable(std::get<ComponentContainer<ComponentTypes> *>(containerViews)->getComponent(entity)...);
                }
            }
        }
    }

    template<typename... ExcludedTypes, typename... ComponentTypes>
    template<typename FunctionType, typename ClassType>
    void ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::forEach(FunctionType function, ClassType *object) {
        for(auto entity : *drivingContainer) {
            if constexpr(sizeof...(ExcludedTypes) > 0) {
                if((std::get<ComponentContainer<ExcludedTypes> *>(excludedContainers)->hasComponent(entity) || ...)) {
                    continue;
                }
            }

            if((std::get<ComponentContainer<ComponentTypes> *>(containerViews)->hasComponent(entity) && ...)) {
                //Call with entity if it's the first argument
                if constexpr(std::is_same_v<Entity, std::tuple_element_t<0, typename FunctionTraits<FunctionType>::ParameterTypesTuple>>) {
                    (object->*function)(entity, std::get<ComponentContainer<ComponentTypes> *>(containerViews)->getComponent(entity)...);
                } else {
                    (object->*function)(std::get<ComponentContainer<ComponentTypes> *>(containerViews)->getComponent(entity)...);
                }
            }
        }
    }

    template<typename... ExcludedTypes, typename... ComponentTypes>
    ComponentContainerInterface *ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...>::chooseDrivingContainer() {
        return std::min(std::initializer_list<ComponentContainerInterface *>{ std::get<ComponentContainer<ComponentTypes> *>(containerViews)... }, [](auto const *const lhs, auto const *const rhs) {
            return lhs->size() < rhs->size();
        });
    }

    template<typename ComponentType>
    ComponentView<Exclude<>, ComponentType>::ComponentView(ComponentContainer<ComponentType> *container)
        : container{ container } {
    }

    template<typename ComponentType>
    ComponentView<Exclude<>, ComponentType>::ComponentView(ComponentView const &other) = default;

    template<typename ComponentType>
    ComponentView<Exclude<>, ComponentType>::ComponentView(ComponentView &&other) noexcept = default;

    template<typename ComponentType>
    ComponentView<Exclude<>, ComponentType> &ComponentView<Exclude<>, ComponentType>::operator=(ComponentView const &other) = default;

    template<typename ComponentType>
    ComponentView<Exclude<>, ComponentType> &ComponentView<Exclude<>, ComponentType>::operator=(ComponentView &&other) noexcept = default;

    template<typename ComponentType>
    ComponentView<Exclude<>, ComponentType>::~ComponentView() = default;

    template<typename ComponentType>
    template<typename CallableType>
    void ComponentView<Exclude<>, ComponentType>::forEach(CallableType callable) {
        //TODO: Just iterate components here?
        for(auto entity : *container) {
            //Call with entity if it's the first argument
            if constexpr(std::is_same_v<Entity, std::tuple_element_t<0, typename FunctionTraits<CallableType>::ParameterTypesTuple>>) {
                callable(entity, container->getComponent(entity));
            } else {
                callable(container->getComponent(entity));
            }
        }
    }

    template<typename ComponentType>
    template<typename FunctionType, typename ClassType>
    void ComponentView<Exclude<>, ComponentType>::forEach(FunctionType function, ClassType *object) {
        for(auto entity : *container) {
            //TODO: Just iterate components here?

            //Call with entity if it's the first argument
            if constexpr(std::is_same_v<Entity, std::tuple_element_t<0, typename FunctionTraits<FunctionType>::ParameterTypesTuple>>) {
                (object->*function)(entity, container->getComponent(entity));
            } else {
                (object->*function)(container->getComponent(entity));
            }
        }
    }
}