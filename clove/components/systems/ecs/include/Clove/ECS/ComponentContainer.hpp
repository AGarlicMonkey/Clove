#pragma once

#include "Clove/ECS/Entity.hpp"

#include <unordered_map>
#include <vector>

namespace clove {
    class EventDispatcher;
}

namespace clove {
    class ComponentContainerInterface {
        //TYPES
    public:
        template<typename T>
        using ContainerType = std::vector<T>;

        using IndexType = typename ContainerType<Entity>::size_type;
        using Iterator  = typename ContainerType<Entity>::iterator;

        //VARIABLES
    protected:
        static inline IndexType constexpr nullIndex{ ~0u };

        ContainerType<IndexType> entityToIndex{}; /**< Sparse vector of indices. Used to index into either packed array. Entity is an index into this array */
        ContainerType<Entity> entities{};         /**< Packed vector of entities. Used when iterating over just the entities this container is aware of. */

        //FUNCTIONS
    public:
        ComponentContainerInterface();

        ComponentContainerInterface(ComponentContainerInterface const &other) = delete;
        ComponentContainerInterface(ComponentContainerInterface &&other) noexcept;

        ComponentContainerInterface &operator=(ComponentContainerInterface const &other) = delete;
        ComponentContainerInterface &operator=(ComponentContainerInterface &&other) noexcept;

        virtual ~ComponentContainerInterface();

        inline bool hasComponent(Entity entity);
        virtual void cloneComponent(Entity from, Entity to) = 0;

        virtual void removeComponent(Entity entity) = 0;

        inline IndexType size() const;

        inline Iterator begin() noexcept;
        inline Iterator end() noexcept;
    };

    template<typename ComponentType>
    class ComponentContainer : public ComponentContainerInterface {
        //VARIABLES
    private:
        ContainerType<ComponentType> components{}; /**< Packed vector of components. Storage for components and used when iterating over just the components this container owns. */

        EventDispatcher *ecsEventDispatcher{ nullptr };

        //FUNCTIONS
    public:
        ComponentContainer() = delete;
        ComponentContainer(EventDispatcher *dispatcher);

        ComponentContainer(ComponentContainer const &other) = delete;
        ComponentContainer(ComponentContainer &&other) noexcept;

        ComponentContainer &operator=(ComponentContainer const &other) = delete;
        ComponentContainer &operator=(ComponentContainer &&other) noexcept;

        ~ComponentContainer();

        template<typename... ConstructArgs>
        ComponentType &addComponent(Entity entity, ConstructArgs &&... args);
        ComponentType &getComponent(Entity entity);

        void cloneComponent(Entity from, Entity to) final;
        void removeComponent(Entity entity) final;
    };
}

#include "ComponentContainer.inl"