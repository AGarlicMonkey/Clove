#include "Clove/Components/TransformComponent.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Maths/Maths.hpp>
#include <Clove/Maths/MathsHelpers.hpp>

namespace clove {
    vec3f TransformComponent::getWorldPosition() const {
        return decomposeTranslation(worldMatrix);
    }

    quatf TransformComponent::getWorldRotation() const {
        return decomposeRotation(worldMatrix);
    }

    vec3f TransformComponent::getWorldScale() const {
        return decomposeScale(worldMatrix);
    }

    vec3f TransformComponent::getForward() const {
        vec3f constexpr worldForward{ 0.0f, 0.0f, 1.0f };
        return normalise(getWorldRotation() * worldForward);
    }

    vec3f TransformComponent::getLeft() const {
        vec3f constexpr worldUp{ 0.0f, 1.0f, 0.0f };
        return normalise(cross(getForward(), worldUp));
    }

    vec3f TransformComponent::getUp() const {
        return normalise(cross(getLeft(), getForward()));
    }
}