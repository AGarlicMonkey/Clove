#pragma once

#include "Clove/Layer.hpp"

#include <Clove/DeltaTime.hpp>
#include <Clove/ECS/ECSEvents.hpp>
#include <Clove/ECS/Entity.hpp>
#include <Clove/Event/EventHandle.hpp>
#include <Clove/Maths/Vector.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionObject;
class btCollisionShape;

namespace clove {
    struct CollisionShapeComponent;
    struct RigidBodyComponent;
    class EntityManager;

    /**
     * @brief Added to an entity when the system knows about it. Contains the Bullet types.
     */
    struct PhysicsProxyComponent {
        std::unique_ptr<btCollisionObject> collisionObject;
        std::unique_ptr<btCollisionShape> collisionShape;
    };
}

namespace clove {
    class PhysicsLayer : public Layer {
        //TYPES
    private:
        struct CollisionManifold {
            Entity entityA;
            Entity entityB;

            constexpr friend bool operator==(CollisionManifold const &lhs, CollisionManifold const &rhs) {
                return (lhs.entityA == rhs.entityA && lhs.entityB == rhs.entityB) ||
                    (lhs.entityA == rhs.entityB && lhs.entityB == rhs.entityA);
            }
            constexpr friend bool operator!=(CollisionManifold const &lhs, CollisionManifold const &rhs) {
                return !(lhs == rhs);
            }
        };

        struct ManifoldHasher {
            size_t operator()(CollisionManifold const &manifold) const {
                size_t a = std::hash<Entity>{}(manifold.entityA);
                size_t b = std::hash<Entity>{}(manifold.entityB);
                return a ^ (b << 1);
            }
        };

        //VARIABLES
    private:
        EntityManager *entityManager{ nullptr };

        std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> dispatcher;
        std::unique_ptr<btBroadphaseInterface> broadphase;
        std::unique_ptr<btSequentialImpulseConstraintSolver> solver;

        std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

        EventHandle proxyRemovedHandle;
        
        std::unordered_set<CollisionManifold, ManifoldHasher> currentCollisions;

        //FUNCTIONS
    public:
		PhysicsLayer() = delete;
		PhysicsLayer(EntityManager *entityManager);

        PhysicsLayer(PhysicsLayer const &other) = delete;
        PhysicsLayer(PhysicsLayer &&other) noexcept;

        PhysicsLayer &operator=(PhysicsLayer const &other) = delete;
        PhysicsLayer &operator=(PhysicsLayer &&other) noexcept;

        ~PhysicsLayer();

        void onUpdate(DeltaTime const deltaTime) override;

        void setGravity(vec3f const &gravity);

        /**
         * @brief Fires a ray into the world.
         * @returns The ID of the first Entity hit.
         */
        Entity rayCast(vec3f const &begin, vec3f const &end);
        /**
         * @brief Fires a ray into the world.
         * @param collisionGroup Bit flag of the collision groups the ray belongs to.
         * @param collisionMask Bit flag of the collision groups the ray collides with.
         * @return The ID of the first Entity hit.
         */
        Entity rayCast(vec3f const &begin, vec3f const &end, uint32_t const collisionGroup, uint32_t const collisionMask);

    private:
        void initialiseCollisionShape(Entity entity, CollisionShapeComponent const &shape);
        void initialiseRigidBody(Entity entity, RigidBodyComponent const &body);
        void initialiseRigidBodyShape(Entity entity, CollisionShapeComponent const &shape, RigidBodyComponent const &body);

        void onProxyRemoved(ComponentRemovedEvent<PhysicsProxyComponent> const &event);
    };
}
