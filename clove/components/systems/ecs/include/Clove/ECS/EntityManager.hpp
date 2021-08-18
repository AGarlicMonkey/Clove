#pragma once

#include "Clove/ECS/ComponentManager.hpp"
#include "Clove/ECS/Entity.hpp"

#include <Clove/Event/EventDispatcher.hpp>
#include <set>
#include <vector>
#include <utility>

namespace clove {
    /**
	 * @brief The EntityManager that contains the Entities and their Components.
	 */
    class EntityManager {
        //VARIABLES
    private:
        EventDispatcher ecsEventDispatcher;

        ComponentManager componentManager;

        static Entity nextEntity;
        std::vector<Entity> activeEntities;
        std::set<Entity> pendingDestroyEntities;

        //FUNCTIONS
    public:
        EntityManager();

        EntityManager(EntityManager const &other)     = delete;
        EntityManager(EntityManager &&other) noexcept = delete;

        EntityManager &operator=(EntityManager const &other) = delete;
        EntityManager &operator=(EntityManager &&other) noexcept = delete;

        ~EntityManager();

        inline EventDispatcher &getDispatcher();

        /**
		 * @brief Creates an entity to be part of this EntityManager.
		 * @return The newly created entity.
		 */
        Entity create();
        /**
		 * @brief Clones an Entiy's Components, returning a new Entity with the same Component layout.
		 * @param ID The Id of the entity to be cloned.
		 * @return The newly cloned entity.
		 */
        Entity clone(Entity entity);

        /**
		 * @brief Removes the Entity from this EntityManager.
		 * @details Entities are removed the next time update is called.
		 * @param ID The Entity to remove.
		 */
        void destroy(Entity entity);
        /**
		 * @brief Destroys all Entities in this EntityManager.
		 * @details Entities are destroyed immediately.
		 */
        void destroyAll();

        /**
		 * @brief Checks if an Entity is valid.
		 * @details An entity is valid if it is part of this EntityManager and has a valid EntityID.
		 * @param ID The Entity to check.
		 * @return Returns true if the entity is valid.
		 */
        bool isValid(Entity entity);

        /**
		 * @brief Adds a Component to this entityManager.
		 * @tparam ComponentType The type of Component to add.
		 * @param entityId The Entity to add the component to.
		 * @param args Construction arguments to be forwarded to the component's constructor.
		 * @return A ComponentPtr to the newly created component.
		 * @see	ComponentPtr.
		 */
        template<typename ComponentType, typename... ConstructArgs>
        ComponentType &addComponent(Entity entity, ConstructArgs &&... args);
        /**
		 * @brief Gets the Component for an Entity.
		 * @tparam ComponentType The type of Component to get.
		 * @param entityId The Entity to get the component from.
		 * @return A ComponentPtr to the component if it exists on the Entity.
		 * @see ComponentPtr.
		 */
        template<typename ComponentType>
        ComponentType &getComponent(Entity entity);
        /**
		 * @brief Checks to see if the component exists on an Entity.
		 * @tparam ComponentType The type of the Component to check for.
		 * @param entityId The entity to check.
		 * @return Returns true if the Component exists on the Entity.
		 */
        template<typename ComponentType>
        bool hasComponent(Entity entity);
        /**
		 * @brief Removes a component from an Entity.
		 * @tparam ComponentType The type of Component to remove.
		 * @param entiyId The Entity to remove the Component from.
		 */
        template<typename ComponentType>
        void removeComponent(Entity entity);

        /**
         * @brief Calls the function for every Entity that has the components of the function arguments.
         */
        template<typename FunctionType, typename... ExcludeTypes>
        void forEach(FunctionType function, Exclude<ExcludeTypes...> = {});

        /**
         * @brief Calls the member function for every Entity that has the components of the function arguments.
         */
        template<typename FunctionType, typename ClassType, typename... ExcludeTypes>
        void forEach(FunctionType function, ClassType *object, Exclude<ExcludeTypes...> = {});

    private:
        template<typename FunctionType, size_t... indices, typename... ExcludeTypes>
        auto generateViewFromFunction(std::index_sequence<indices...>, Exclude<ExcludeTypes...> = {});

        template<typename ComponentType, typename... ComponentTypes, typename... ExcludeTypes>
        auto generateView(Exclude<ExcludeTypes...> = {});
    };
}

#include "EntityManager.inl"
