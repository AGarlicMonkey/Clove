#pragma once

#include "Clove/SerialisationCommon.hpp"

#include <Clove/Maths/MathsTypes.hpp>

namespace clove {
    /**
     * @brief A TransformComponent holds the position, rotation and scale of an Entity.
     */
    struct TransformComponent {
        vec3f position{ 0.0f, 0.0f, 0.0f };          /**< The local position of this transform. */
        quatf rotation{ vec3f{ 0.0f, 0.0f, 0.0f } }; /**< The local rotation of this transform. */
        vec3f scale{ 1.0f, 1.0f, 1.0f };             /**< The local scale of this transform. */

        /**
         * @brief Contains the world matrix which this transforms local matrix multiplied 
         * by it's parents matrix (if available).
         * @details This matrix is calculated inside TransformLayer. One frame needs to pass
         * before it is updated
         */
        mat4f worldMatrix{ 1.0f };

        /**
         * @brief Extracts the position from worldMatrix.
         * @return 
         */
        vec3f getWorldPosition() const;
        /**
         * @brief Extracts the rotation from worldMatrix.
         * @return 
         */
        quatf getWorldRotation() const;
        /**
         * @brief Extracts the scale from worldMatrix.
         * @return 
         */
        vec3f getWorldScale() const;

        vec3f getForward() const;
        vec3f getLeft() const;
        vec3f getUp() const;
    };
}

namespace clove {
    template<>
    inline serialiser::Node serialise(TransformComponent const &object) {
        serialiser::Node node{};
        node["position"] = object.position;
        node["rotation"] = object.rotation;
        node["scale"]    = object.scale;
        return node;
    }

    template<>
    inline TransformComponent deserialise(serialiser::Node const &node) {
        TransformComponent component{};
        component.position = node["position"].as<vec3f>();
        component.rotation = node["rotation"].as<quatf>();
        component.scale    = node["scale"].as<vec3f>();
        return component;
    }
}