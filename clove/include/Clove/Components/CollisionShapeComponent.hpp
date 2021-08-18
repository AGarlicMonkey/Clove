#pragma once

#include "Clove/SerialisationCommon.hpp"

#include <Clove/Maths/Vector.hpp>
#include <variant>

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

namespace clove {
    enum class ShapeSeralisationType : uint32_t {
        Sphere = 0,
        Cube   = 1,
    };

    template<>
    inline serialiser::Node serialise(CollisionShapeComponent const &object) {
        serialiser::Node node{};
        if(auto *sphere{ std::get_if<CollisionShapeComponent::Sphere>(&object.shape) }) {
            node["type"]  = static_cast<uint32_t>(ShapeSeralisationType::Sphere);
            node["value"] = sphere->radius;
        } else if(auto *cube{ std::get_if<CollisionShapeComponent::Cube>(&object.shape) }) {
            node["type"]  = static_cast<uint32_t>(ShapeSeralisationType::Cube);
            node["value"] = cube->halfExtents;
        }
        return node;
    }

    template<>
    inline CollisionShapeComponent deserialise(serialiser::Node const &node) {
        CollisionShapeComponent component{};
        ShapeSeralisationType const shapeType{ node["type"].as<uint32_t>() };
        switch(shapeType) {
            case ShapeSeralisationType::Sphere:
                component.shape = CollisionShapeComponent::Sphere{ node["value"].as<float>() };
                break;
            case ShapeSeralisationType::Cube:
                component.shape = CollisionShapeComponent::Cube{ node["value"].as<vec3f>() };
                break;
            default:
                break;
        }
        return component;
    }
}