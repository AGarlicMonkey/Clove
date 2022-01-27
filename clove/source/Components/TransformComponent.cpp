#include "Clove/Components/TransformComponent.hpp"

#include "Clove/ReflectionAttributes.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/ECS/EntityManager.hpp>
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

CLOVE_REFLECT_BEGIN(clove::TransformComponent, clove::EditorVisibleComponent{
                                                   .name                    = "Transform Component",
                                                   .onEditorCreateComponent = [](clove::Entity entity, clove::EntityManager &manager) -> uint8_t * {
                                                       return reinterpret_cast<uint8_t *>(&manager.addComponent<clove::TransformComponent>(entity));
                                                   },
                                                   .onEditorGetComponent = [](clove::Entity entity, clove::EntityManager &manager) -> uint8_t * {
                                                       return reinterpret_cast<uint8_t *>(&manager.getComponent<clove::TransformComponent>(entity));
                                                   },
                                                   .onEditorDestroyComponent = [](clove::Entity entity, clove::EntityManager &manager) { manager.removeComponent<clove::TransformComponent>(entity); },
                                               })
CLOVE_REFLECT_MEMBER(position, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(rotation, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(scale, clove::EditorEditableMember{})
CLOVE_REFLECT_END