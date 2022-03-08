#pragma once

#include <Clove/Maths/Vector.hpp>
#include <cinttypes>
#include <optional>
#include <Clove/Reflection/Reflection.hpp>

namespace clove {
    /**
	 * @brief Enables an entity to respond to physics events.
	 * @details Entities with a RigidBodyComponent can collide with other
	 * entities and be affected by gravity. If the entity also has a
	 * CollisionShapeComponent then the rigid body will use that shape
	 * to detect collisions.
	 */
    struct RigidBodyComponent {
        friend class PhysicsSubSystem;

    private:
        struct ForceApplication {
            vec3f amount{ 0.0f, 0.0f, 0.0f };
            vec3f offset{ 0.0f, 0.0f, 0.0f };
        };

    public:
        /**
         * @brief Bit flag of the collision groups this body is a part of.
         */
        uint32_t collisionGroup{ ~0u };
        /**
         * @brief Bit flag of which collision groups this body collides with.
         */
        uint32_t collisionMask{ ~0u };

        /**
         * @brief The mass of the body. If 0 then the body will be able to be collided with but not respond to physics.
         */
        float mass{ 1.0f };

        /**
         * @brief The bouncyness of this body. Anything higher than 0 will cause the body to increase in bouncyness.
         */
        float restitution{ 0.0f };

        /**
         * @brief Acts as a multiplier to any rotational forces applied to this body.
         */
        vec3f angularFactor{ 1.0f, 1.0f, 1.0f };
        /**
         * @brief Acts as a multiplier to any linear forces applied to this body.
         */
        vec3f linearFactor{ 1.0f, 1.0f, 1.0f };

        /**
         * @brief Sets a consistent velocity for the body to move in.
         */
        void setLinearVelocity(vec3f velocity) {
            appliedVelocity = velocity;
        }

        /**
         * @brief Returns the current velocity of the body
         */
        vec3f getLinearVelocity() const {
            return currentVelocity;
        }

        /**
         * @brief Apply a consistent force to this body.
         * @param force The direction + power of the force.
         * @param relativeOffset The offset in relation to the body's center of mass.
         */
        void applyForce(vec3f force, vec3f relativeOffset = { 0.0f, 0.0f, 0.0f });

        /**
         * @brief Apply a 1 frame burst of force to this body.
         * @param impulse The direction + power of the impulse.
         * @param relativeOffset The offset in relation to the body's center of mass.
         */
        void applyImpulse(vec3f impulse, vec3f relativeOffset = { 0.0f, 0.0f, 0.0f });

    private:
        std::optional<vec3f> appliedVelocity{};

        std::optional<ForceApplication> appliedForce{};
        std::optional<ForceApplication> appliedImpulse{};

        vec3f currentVelocity{};
    };
}

CLOVE_REFLECT_DECLARE_TYPE(clove::RigidBodyComponent)