#pragma once

#include <Bulb/ECS/Component.hpp>
#include <Bulb/ECS/Entity.hpp>
#include <Root/Delegate/MultiCastDelegate.hpp>

namespace garlic::inline stem {
    struct Collision {
        blb::ecs::Entity entity;   /**< 'this' Entity. */
        blb::ecs::Entity collider; /**< The Entity that collided with this entity */
    };

    /**
     * @brief A CollisionResponseComponent exposes collision callbacks for an Entity.
     * @details Everytime a callback is triggered means another entity has collided
     * with this entity. Note that on it's own this component does nothing. The Entity
     * needs a RigidBodyComponent or a ColliderComponent to response to collisions.
     */
    class CollisionResponseComponent : public blb::ecs::Component<CollisionResponseComponent> {
        //VARIABLES
    public:
        /**
         * @brief Called on the frame the collision event starts.
         */
        garlic::MultiCastDelegate<void(Collision const &)> onCollisionBegin;
        /**
         * @brief Called on the frame the collision event ends.
         */
        garlic::MultiCastDelegate<void(Collision const &)> onCollisionEnd;
    };
}