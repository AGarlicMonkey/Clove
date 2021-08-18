#include "Clove/Layers/PhysicsLayer.hpp"

#include "Clove/Application.hpp"
#include "Clove/Components/CollisionResponseComponent.hpp"
#include "Clove/Components/CollisionShapeComponent.hpp"
#include "Clove/Components/RigidBodyComponent.hpp"
#include "Clove/Components/TransformComponent.hpp"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <Clove/Cast.hpp>
#include <Clove/Delegate/MultiCastDelegate.hpp>
#include <Clove/Delegate/SingleCastDelegate.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Event/EventDispatcher.hpp>
#include <btBulletDynamicsCommon.h>

namespace clove {
    namespace {
        template<typename... Ts>
        struct match : Ts... { using Ts::operator()...; };
        template<typename... Ts>
        match(Ts...) -> match<Ts...>;

        /**
         * @brief Tag component for entities that physics knows only about it's shape.
         */
        struct ShapeOnlyComponent {};
        /**
         * @brief Tag component for entities that physics knows only about it's rigid body.
         */
        struct BodyOnlyComponent {};

        /**
         * @brief Initialises proxy with the correct shape.
         */
        void createProxyShape(PhysicsProxyComponent &proxy, CollisionShapeComponent const &shape) {
            std::visit(match{
                           [&](CollisionShapeComponent::Sphere const &sphere) { proxy.collisionShape = std::make_unique<btSphereShape>(sphere.radius); },
                           [&](CollisionShapeComponent::Cube const &cube) { proxy.collisionShape = std::make_unique<btBoxShape>(btVector3{ cube.halfExtents.x, cube.halfExtents.y, cube.halfExtents.z }); },
                       },
                       shape.shape);
        }

        /**
         * @brief Initialises proxy with a btRigidBody.
         * @return The created body so it can be added to the world.
         */
        btRigidBody *createProxyBody(PhysicsProxyComponent &proxy, RigidBodyComponent const &body, Entity entity) {
            btVector3 localInertia{ 0.0f, 0.0f, 0.0f };
            if(body.mass > 0.0f) {
                proxy.collisionShape->calculateLocalInertia(body.mass, localInertia);
            }

            auto proxyBody{ std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{ body.mass, nullptr, proxy.collisionShape.get(), localInertia }) };
            btRigidBody *rawBody{ proxyBody.get() };

            proxy.collisionObject = std::move(proxyBody);
            proxy.collisionObject->setUserIndex(static_cast<int>(entity));

            return rawBody;
        }

        btVector3 toBt(vec3f const &vec) {
            return btVector3{ vec.x, vec.y, vec.z };
        }

        vec3f toGar(btVector3 const &vec) {
            return vec3f{ vec.getX(), vec.getY(), vec.getZ() };
        }
    }

    PhysicsLayer::PhysicsLayer(EntityManager *entityManager)
        : Layer("Physics")
        , entityManager{ entityManager } {
        collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
        dispatcher             = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
        broadphase             = std::make_unique<btDbvtBroadphase>();
        solver                 = std::make_unique<btSequentialImpulseConstraintSolver>();

        dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());

        proxyRemovedHandle = entityManager->getDispatcher().bindToEvent<ComponentRemovedEvent<PhysicsProxyComponent>>([this](ComponentRemovedEvent<PhysicsProxyComponent> const &event) {
            onProxyRemoved(event);
        });
    }

    PhysicsLayer::PhysicsLayer(PhysicsLayer &&other) noexcept = default;

    PhysicsLayer &PhysicsLayer::operator=(PhysicsLayer &&other) noexcept = default;

    PhysicsLayer::~PhysicsLayer() = default;

    void PhysicsLayer::onUpdate(DeltaTime const deltaTime) {
        CLOVE_PROFILE_FUNCTION();

        std::vector<Entity> proxiesToRemove{};
        std::vector<Entity> shapeTagsToRemove{};
        std::vector<Entity> bodyTagsToRemove{};
        //If any component compositions have changed then these need to be reset
        entityManager->forEach([&](Entity entity, CollisionShapeComponent const &shape, RigidBodyComponent const &body, PhysicsProxyComponent const &proxy, ShapeOnlyComponent const &shapeOnly) {
            proxiesToRemove.push_back(entity);
            shapeTagsToRemove.push_back(entity);
        });
        entityManager->forEach([&](Entity entity, CollisionShapeComponent const &shape, RigidBodyComponent const &body, PhysicsProxyComponent const &proxy, BodyOnlyComponent const &shapeOnly) {
            proxiesToRemove.push_back(entity);
            bodyTagsToRemove.push_back(entity);
        });
        entityManager->forEach([&](Entity entity, PhysicsProxyComponent const &proxy) {
            if(!entityManager->hasComponent<CollisionShapeComponent>(entity) || !entityManager->hasComponent<RigidBodyComponent>(entity)) {
                proxiesToRemove.push_back(entity);
            }
        }, Exclude<ShapeOnlyComponent, BodyOnlyComponent>{});

        //Remove them outside of the loops their found in.
        for(auto entity : proxiesToRemove) {
            entityManager->removeComponent<PhysicsProxyComponent>(entity);
        }
        for(auto entity : shapeTagsToRemove) {
            entityManager->removeComponent<ShapeOnlyComponent>(entity);
        }
        for(auto entity : bodyTagsToRemove) {
            entityManager->removeComponent<BodyOnlyComponent>(entity);
        }

        std::vector<Entity> requiresCollisionShape{};
        std::vector<Entity> requiresRigidBody{};
        std::vector<Entity> requiresRigidBodyShape{};
        //Create proxies for any new / modified entities
        entityManager->forEach([&](Entity entity, CollisionShapeComponent const &shape) { requiresCollisionShape.push_back(entity); }, Exclude<PhysicsProxyComponent, RigidBodyComponent>{});
        entityManager->forEach([&](Entity entity, RigidBodyComponent const &rigidBody) { requiresRigidBody.push_back(entity); }, Exclude<PhysicsProxyComponent, CollisionShapeComponent>{});
        entityManager->forEach([&](Entity entity, CollisionShapeComponent const &shape, RigidBodyComponent const &body) { requiresRigidBodyShape.push_back(entity); }, Exclude<PhysicsProxyComponent>{});

        for(auto entity : requiresCollisionShape) {
            initialiseCollisionShape(entity, entityManager->getComponent<CollisionShapeComponent>(entity));
        }
        for(auto entity : requiresRigidBody) {
            initialiseRigidBody(entity, entityManager->getComponent<RigidBodyComponent>(entity));
        }
        for(auto entity : requiresRigidBodyShape) {
            initialiseRigidBodyShape(entity, entityManager->getComponent<CollisionShapeComponent>(entity), entityManager->getComponent<RigidBodyComponent>(entity));
        }

        //Update any shapes that would've changed
        entityManager->forEach([&](CollisionShapeComponent const &shape, PhysicsProxyComponent &proxy) {
            bool needsNewShape{ false };

            if(std::holds_alternative<CollisionShapeComponent::Sphere>(shape.shape)) {
                if(proxy.collisionShape->getShapeType() == BroadphaseNativeTypes::SPHERE_SHAPE_PROXYTYPE) {
                    static_cast<btSphereShape *>(proxy.collisionShape.get())->setUnscaledRadius(std::get<CollisionShapeComponent::Sphere>(shape.shape).radius);//NOLINT RTTI is not included in bullet
                } else {
                    //Shape type mismatch - recreate the proxy's body
                    needsNewShape = true;
                }
            } else if(std::holds_alternative<CollisionShapeComponent::Cube>(shape.shape)) {
                if(proxy.collisionShape->getShapeType() == BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE) {
                    auto const halfExtents{ std::get<CollisionShapeComponent::Cube>(shape.shape).halfExtents };
                    static_cast<btBoxShape *>(proxy.collisionShape.get())->setImplicitShapeDimensions(btVector3{ halfExtents.x, halfExtents.y, halfExtents.z });//NOLINT RTTI is not included in bullet
                } else {
                    //Shape type mismatch - recreate the proxy's body
                    needsNewShape = true;
                }
            }

            if(needsNewShape) {
                createProxyShape(proxy, shape);
            }
        });

        //Apply any updates to the rigid body
        entityManager->forEach([&](RigidBodyComponent &body, PhysicsProxyComponent const &proxy) {
            auto *btBody{ static_cast<btRigidBody *>(proxy.collisionObject.get()) };//NOLINT RTTI is not included in bullet

            if(body.appliedVelocity.has_value()) {
                btBody->setLinearVelocity(toBt(*body.appliedVelocity));
                btBody->activate();
                body.appliedVelocity = std::nullopt;
            }

            btBody->setRestitution(body.restitution);

            btBody->setAngularFactor(toBt(body.angularFactor));
            btBody->setLinearFactor(toBt(body.linearFactor));

            if(body.appliedForce.has_value()) {
                btBody->applyForce(toBt(body.appliedForce->amount), toBt(body.appliedForce->offset));
                btBody->activate();
                body.appliedForce = std::nullopt;
            }

            if(body.appliedImpulse.has_value()) {
                btBody->applyForce(toBt(body.appliedImpulse->amount), toBt(body.appliedImpulse->offset));
                btBody->activate();
                body.appliedImpulse = std::nullopt;
            }
        });

        //Notify Bullet of the location of the colliders
        entityManager->forEach([&](TransformComponent const &transform, PhysicsProxyComponent const &proxy) {
            auto const &pos{ transform.position };
            auto const &rot{ transform.rotation };
            auto const &scale{ transform.scale };

            btTransform btTrans{ proxy.collisionObject->getWorldTransform() };
            btTrans.setOrigin({ pos.x, pos.y, pos.z });
            btTrans.setRotation({ rot.x, rot.y, rot.z, rot.w });
            proxy.collisionObject->setWorldTransform(btTrans);

            if(proxy.collisionShape != nullptr) {
                proxy.collisionShape->setLocalScaling(btVector3{ scale.x, scale.y, scale.z });
            }
        });

        //Step physics world
        dynamicsWorld->stepSimulation(deltaTime.getDeltaSeconds());

        //Apply any simulation updates
        entityManager->forEach([&](TransformComponent &transform, PhysicsProxyComponent const &proxy) {
            btTransform const &btTrans{ proxy.collisionObject->getWorldTransform() };
            btVector3 const &pos{ btTrans.getOrigin() };
            btQuaternion const &rot{ btTrans.getRotation() };

            transform.position = vec3f{ pos.x(), pos.y(), pos.z() };
            transform.rotation = quatf{ rot.getW(), rot.getX(), rot.getY(), rot.getZ() };
        });
        entityManager->forEach([](RigidBodyComponent &body, PhysicsProxyComponent const &proxy) {
            auto *btBody{ static_cast<btRigidBody *>(proxy.collisionObject.get()) };//NOLINT
            body.currentVelocity = toGar(btBody->getLinearVelocity());
        });

        //Gather collision manifolds
        int const numManifolds = dispatcher->getNumManifolds();
        std::unordered_set<CollisionManifold, ManifoldHasher> newCollisions;

        for(int i = 0; i < numManifolds; ++i) {
            btPersistentManifold *manifold = dispatcher->getManifoldByIndexInternal(i);
            if(manifold->getNumContacts() > 0) {
                btCollisionObject const *obA{ manifold->getBody0() };
                btCollisionObject const *obB{ manifold->getBody1() };

                auto const entityA{ static_cast<Entity>(obA->getUserIndex()) };
                auto const entityB{ static_cast<Entity>(obB->getUserIndex()) };

                newCollisions.emplace(CollisionManifold{ entityA, entityB });
            }
        }

        std::vector<CollisionManifold> collisionBeginManifolds;
        std::vector<CollisionManifold> collisionEndManifolds;

        //Copy any elements not in our cached set
        collisionBeginManifolds.reserve(std::size(newCollisions));
        std::copy_if(std::begin(newCollisions), std::end(newCollisions), std::back_inserter(collisionBeginManifolds), [this](CollisionManifold const &manifold) {
            return !currentCollisions.contains(manifold);
        });

        //Copy any elements not in our new array
        collisionEndManifolds.reserve(std::size(currentCollisions));
        std::copy_if(std::begin(currentCollisions), std::end(currentCollisions), std::back_inserter(collisionEndManifolds), [&](CollisionManifold const &manifold) {
            return !newCollisions.contains(manifold);
        });

        //Broadcast collision begin events
        for(auto const &manifold : collisionBeginManifolds) {
            Entity const entityA{ manifold.entityA };
            Entity const entityB{ manifold.entityB };

            if(entityManager->hasComponent<CollisionResponseComponent>(entityA)) {
                entityManager->getComponent<CollisionResponseComponent>(entityA).onCollisionBegin.broadcast(Collision{ entityA, entityB });
            }

            if(entityManager->hasComponent<CollisionResponseComponent>(entityB)) {
                entityManager->getComponent<CollisionResponseComponent>(entityB).onCollisionBegin.broadcast(Collision{ entityB, entityA });
            }

            currentCollisions.emplace(manifold);
        }

        //Broadcast collision end events
        for(auto const &manifold : collisionEndManifolds) {
            Entity const entityA{ manifold.entityA };
            Entity const entityB{ manifold.entityB };

            if(entityManager->hasComponent<CollisionResponseComponent>(entityA)) {
                entityManager->getComponent<CollisionResponseComponent>(entityA).onCollisionEnd.broadcast(Collision{ entityA, entityB });
            }

            if(entityManager->hasComponent<CollisionResponseComponent>(entityB)) {
                entityManager->getComponent<CollisionResponseComponent>(entityB).onCollisionEnd.broadcast(Collision{ entityB, entityA });
            }

            currentCollisions.erase(manifold);
        }
    }

    void PhysicsLayer::setGravity(vec3f const &gravity) {
        dynamicsWorld->setGravity({ gravity.x, gravity.y, gravity.z });
    }

    Entity PhysicsLayer::rayCast(vec3f const &begin, vec3f const &end) {
        return rayCast(begin, end, ~0, ~0);
    }

    Entity PhysicsLayer::rayCast(vec3f const &begin, vec3f const &end, uint32_t const collisionGroup, uint32_t const collisionMask) {
        btVector3 const btBegin{ begin.x, begin.y, begin.z };
        btVector3 const btEnd{ end.x, end.y, end.z };

        btCollisionWorld::ClosestRayResultCallback callBack{ btBegin, btEnd };
        callBack.m_collisionFilterGroup = static_cast<int>(collisionGroup);
        callBack.m_collisionFilterMask  = static_cast<int>(collisionMask);
        dynamicsWorld->rayTest(btBegin, btEnd, callBack);

        if(callBack.m_collisionObject != nullptr) {
            return callBack.m_collisionObject->getUserIndex();
        } else {
            return NullEntity;
        }
    }

    void PhysicsLayer::initialiseCollisionShape(Entity entity, CollisionShapeComponent const &shape) {
        PhysicsProxyComponent proxy{
            .collisionObject = std::make_unique<btGhostObject>(),
        };

        createProxyShape(proxy, shape);

        //Manually set the properties of the dummy collision object
        proxy.collisionObject->setCollisionShape(proxy.collisionShape.get());
        proxy.collisionObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
        proxy.collisionObject->setUserIndex(static_cast<int>(entity));

        dynamicsWorld->addCollisionObject(proxy.collisionObject.get(), ~0, ~0);//Add the collider to every group and collide with every other group

        entityManager->addComponent<PhysicsProxyComponent>(entity, std::move(proxy));
        entityManager->addComponent<ShapeOnlyComponent>(entity);
    }

    void PhysicsLayer::initialiseRigidBody(Entity entity, RigidBodyComponent const &body) {
        float constexpr defaultSphereSize{ 0.1f };
        PhysicsProxyComponent proxy{
            .collisionShape = std::make_unique<btSphereShape>(defaultSphereSize),
        };

        btRigidBody *rawBody{ createProxyBody(proxy, body, entity) };

        dynamicsWorld->addRigidBody(rawBody, static_cast<int>(body.collisionGroup), static_cast<int>(body.collisionMask));

        entityManager->addComponent<PhysicsProxyComponent>(entity, std::move(proxy));
        entityManager->addComponent<BodyOnlyComponent>(entity);
    }

    void PhysicsLayer::initialiseRigidBodyShape(Entity entity, CollisionShapeComponent const &shape, RigidBodyComponent const &body) {
        PhysicsProxyComponent proxy{};

        createProxyShape(proxy, shape);
        btRigidBody *rawBody{ createProxyBody(proxy, body, entity) };

        dynamicsWorld->addRigidBody(rawBody, static_cast<int>(body.collisionGroup), static_cast<int>(body.collisionMask));

        entityManager->addComponent<PhysicsProxyComponent>(entity, std::move(proxy));
    }

    void PhysicsLayer::onProxyRemoved(ComponentRemovedEvent<PhysicsProxyComponent> const &event) {
        dynamicsWorld->removeCollisionObject(event.component.collisionObject.get());
    }
}
