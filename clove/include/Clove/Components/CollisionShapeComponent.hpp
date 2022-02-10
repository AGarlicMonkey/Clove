#pragma once

#include <Clove/Maths/Vector.hpp>
#include <variant>
#include <Clove/Reflection/Reflection.hpp>

namespace clove {
    /**
     * @brief Provides a collision shape for an entity.
     * @details On it's own a CollisionShapeComponent can act like a trigger.
     * If an Entity also has a RigidBodyComponent then the Entity will respond
     * to gravity and other physics events using this component as it's collision
     * shape.
     * @see CollisionResponseComponent
     * @see RigidBodyComponent
     */
    struct CollisionShapeComponent {
        struct Sphere {
            float radius{ 1.0f };
        };
        struct Cube {
            vec3f halfExtents{ 1.0f, 1.0f, 1.0f };
        };
        using ShapeVariant = std::variant<Sphere, Cube>;

        ShapeVariant shape{ Sphere{} };//Provide a constructed shape to work around a problem on g++/clang 10
    };
}

CLOVE_REFLECT_DECLARE_TYPE(clove::CollisionShapeComponent::Sphere)
CLOVE_REFLECT_DECLARE_TYPE(clove::CollisionShapeComponent::Cube)
CLOVE_REFLECT_DECLARE_TYPE(clove::CollisionShapeComponent)