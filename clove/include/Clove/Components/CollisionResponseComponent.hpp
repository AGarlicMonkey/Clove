#pragma once

#include <Clove/Delegate/MultiCastDelegate.hpp>
#include <Clove/ECS/Entity.hpp>

namespace clove {
    struct Collision {
        Entity entity;   /**< 'this' Entity. */
        Entity collider; /**< The Entity that collided with this entity */
    };

    /**
     * @brief A CollisionResponseComponent exposes collision callbacks for an Entity.
     * @details Everytime a callback is triggered means another entity has collided
     * with this entity. Note that on it's own this component does nothing. The Entity
     * needs a RigidBodyComponent or a CollisionShapeComponent to response to collisions.
     */
    struct CollisionResponseComponent {
        /**
         * @brief Called on the frame the collision event starts.
         */
        MultiCastDelegate<void(Collision const &)> onCollisionBegin;
        /**
         * @brief Called on the frame the collision event ends.
         */
        MultiCastDelegate<void(Collision const &)> onCollisionEnd;
    };
}