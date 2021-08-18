#include "Clove/Layers/TransformLayer.hpp"

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
                if(entityManager->hasComponent<ParentComponent>(parent)){
                    parentsParent = entityManager->getComponent<ParentComponent>(parent).parent;
                }

                return calculateWorldMatrix(entityManager, parentTransform, parentsParent) * transform.worldMatrix;
            } else {
                return transform.worldMatrix;
            }
        }
    }

    TransformLayer::TransformLayer(EntityManager *entityManager)
        : Layer("Transform")
        , entityManager{ entityManager } {
    }

    TransformLayer::TransformLayer(TransformLayer &&other) noexcept = default;

    TransformLayer &TransformLayer::operator=(TransformLayer &&other) noexcept = default;

    TransformLayer::~TransformLayer() = default;

    void TransformLayer::onUpdate(DeltaTime const deltaTime) {
        CLOVE_PROFILE_FUNCTION();

        //Calculate the world matrix of every single transform
        entityManager->forEach([](TransformComponent &transform) {
            mat4f const translationMatrix{ translate(mat4f{ 1.0f }, transform.position) };
            mat4f const rotationMatrix{ quaternionToMatrix4(transform.rotation) };
            mat4f const scaleMatrix{ scale(mat4f{ 1.0f }, transform.scale) };

            transform.worldMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        });

        //Re calculate the world matrix for all transforms with parents
        entityManager->forEach([entityMan = entityManager](TransformComponent &transform, ParentComponent const &parent) {
            transform.worldMatrix = calculateWorldMatrix(entityMan, transform, parent.parent);
        });
    }
}
