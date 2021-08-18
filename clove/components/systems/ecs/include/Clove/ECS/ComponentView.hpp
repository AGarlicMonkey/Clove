#pragma once

#include "Clove/ECS/ComponentContainer.hpp"
#include "Clove/ECS/FunctionTraits.hpp"
#include "Clove/ECS/Exclude.hpp"

#include <tuple>

namespace clove {
    template<typename...>
    class ComponentView;

    /**
     * @brief Provides a view over component containers. Allowing a quick way to iterate over sets of components that share entities.
     * @details Multi-Component view specialisation. 
     * @tparam ExcludedTypes The types of components to exclude. When iterating any entities that have these will be skipped.
     * @tparam ComponentTypes The types of components to view. When iterating only entities with these components will be accessed. 
     */
    template<typename... ExcludedTypes, typename... ComponentTypes>
    class ComponentView<Exclude<ExcludedTypes...>, ComponentTypes...> {
        //VARIABLES
    private:
        std::tuple<ComponentContainer<ComponentTypes> *...> containerViews{};
        std::tuple<ComponentContainer<ExcludedTypes> *...> excludedContainers{};

        ComponentContainerInterface *drivingContainer{ nullptr }; /**< Used to index into other views. Ideally the container with the least entities */

        //FUNCTIONS
    public:
        ComponentView() = delete;
        ComponentView(std::tuple<ComponentContainer<ComponentTypes> *...> containerViews);
        ComponentView(std::tuple<ComponentContainer<ComponentTypes> *...> containerViews, std::tuple<ComponentContainer<ExcludedTypes> *...> excludedContainers);

        ComponentView(ComponentView const &other);
        ComponentView(ComponentView &&other) noexcept;

        ComponentView &operator=(ComponentView const &other);
        ComponentView &operator=(ComponentView &&other) noexcept;

        ~ComponentView();

        /**
         * @brief Call a callable on each component set in the view.
         */
        template<typename CallableType>
        void forEach(CallableType callable);

        /**
         * @brief Call a member function on each component set in the view.
         */
        template<typename FunctionType, typename ClassType>
        void forEach(FunctionType function, ClassType *object);

    private:
        ComponentContainerInterface *chooseDrivingContainer();
    };

    /**
     * @brief Provides a view over component container. Allowing a quick way to iterate over all components of that type.
     * @details Single-Component view specialisation. Provided to slightly optimise single component variadic template arguments. 
     * @tparam ComponentType The component type to view. When iterating only entities with this component will be accessed. 
     */
    template<typename ComponentType>
    class ComponentView<Exclude<>, ComponentType> {
        //VARIABLES
    private:
        ComponentContainer<ComponentType> *container{};

        //FUNCTIONS
    public:
        ComponentView() = delete;
        ComponentView(ComponentContainer<ComponentType> *container);

        ComponentView(ComponentView const &other);
        ComponentView(ComponentView &&other) noexcept;

        ComponentView &operator=(ComponentView const &other);
        ComponentView &operator=(ComponentView &&other) noexcept;

        ~ComponentView();

        /**
         * @brief Call a callable on each component set in the view.
         */
        template<typename CallableType>
        void forEach(CallableType callable);

        /**
         * @brief Call a member function  on each component set in the view.
         */
        template<typename FunctionType, typename ClassType>
        void forEach(FunctionType function, ClassType *object);
    };

    //Deduction guidline for non excluding views.
    template<typename... Ts>
    ComponentView(Ts...) -> ComponentView<Exclude<>, Ts...>;
}

#include "ComponentView.inl"