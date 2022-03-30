#include "Clove/SubSystems/TransformSubSystem.hpp"

#include "Clove/Application.hpp"
#include "Clove/Components/ParentComponent.hpp"
#include "Clove/Components/TransformComponent.hpp"

#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Maths/Maths.hpp>
#include <Clove/Maths/MathsHelpers.hpp>

namespace clove {
    namespace {
        mat4f calculateWorldMatrix(EntityManager *entityManager, TransformComponent &transform, Entity parent) {
            if(entityManager->hasComponent<TransformComponent>(parent)) {
                auto &parentTransform{ entityManager->getComponent<TransformComponent>(parent) };

                Entity parentsParent{ NullEntity };
                if(entityManager->hasComponent<ParentComponent>(parent)) {
                    parentsParent = entityManager->getComponent<ParentComponent>(parent).parent;
                }

                return calculateWorldMatrix(entityManager, parentTransform, parentsParent) * transform.worldMatrix;
            } else {
                return transform.worldMatrix;
            }
        }
    }

    TransformSubSystem::TransformSubSystem(EntityManager *entityManager)
        : SubSystem("Transform")
        , entityManager{ entityManager } {
    }

    TransformSubSystem::TransformSubSystem(TransformSubSystem &&other) noexcept = default;

    TransformSubSystem &TransformSubSystem::operator=(TransformSubSystem &&other) noexcept = default;

    TransformSubSystem::~TransformSubSystem() = default;

    SubSystem::Group TransformSubSystem::getGroup() const {
        return Group::Initialisation;
    }

    void TransformSubSystem::onUpdate(DeltaTime const deltaTime) {
        CLOVE_PROFILE_FUNCTION();

        //Calculate the world matrix of every single transform
        entityManager->forEach([](TransformComponent &transform) {
            vec3f const radRot{ asRadians(transform.rotation) };

            mat4f const translationMatrix{ translate(mat4f{ 1.0f }, transform.position) };
            mat4f const rotationMatrix{ quaternionToMatrix4(quatf{ radRot }) };
            mat4f const scaleMatrix{ scale(mat4f{ 1.0f }, transform.scale) };

            transform.worldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        });

        //Re calculate the world matrix for all transforms with parents
        entityManager->forEach([entityMan = entityManager](TransformComponent &transform, ParentComponent const &parent) {
            transform.worldMatrix = calculateWorldMatrix(entityMan, transform, parent.parent);
        });
    }
}
