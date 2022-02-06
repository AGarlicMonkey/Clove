#include "Clove/SubSystems/RenderSubSystem.hpp"

#include "Clove/Application.hpp"
#include "Clove/Components/AnimatedModelComponent.hpp"
#include "Clove/Components/CameraComponent.hpp"
#include "Clove/Components/DirectionalLightComponent.hpp"
#include "Clove/Components/PointLightComponent.hpp"
#include "Clove/Components/StaticModelComponent.hpp"
#include "Clove/Components/TransformComponent.hpp"
#include "Clove/Rendering/Renderables/Mesh.hpp"
#include "Clove/Rendering/Renderer.hpp"

#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Maths/Maths.hpp>
#include <Clove/Maths/MathsHelpers.hpp>

namespace clove {
    RenderSubSystem::RenderSubSystem(Renderer *renderer, EntityManager *entityManager)
        : SubSystem("Render")
        , renderer{ renderer }
        , entityManager{ entityManager } {
    }

    RenderSubSystem::RenderSubSystem(RenderSubSystem &&other) noexcept = default;

    RenderSubSystem &RenderSubSystem::operator=(RenderSubSystem &&other) noexcept = default;

    RenderSubSystem::~RenderSubSystem() = default;

    SubSystem::Group RenderSubSystem::getGroup() const {
        return Group::Render;
    }

    void RenderSubSystem::onUpdate(DeltaTime const deltaTime) {
        CLOVE_PROFILE_FUNCTION();

        Entity activeCamera{};

        //Transform and submit cameras
        entityManager->forEach([this, &activeCamera](Entity entity, TransformComponent const &transform, CameraComponent &camera) {
            if(camera.isPriority || activeCamera == NullEntity) {
                vec3f const position{ transform.getWorldPosition() };

                vec3f const camFront{ transform.getForward() };
                vec3f const camUp{ transform.getUp() };

                mat4f const view{ lookAt(position, position + camFront, camUp) };
                mat4f const projection{ camera.camera.getProjection(renderer->getRenderTargetSize()) };

                renderer->submitCamera(view, projection, position);

                activeCamera = entity;
            }
        });

        //Submit static meshes
        entityManager->forEach([this](TransformComponent const &transform, StaticModelComponent const &staticModel) {
            if(staticModel.model.isValid()) {
                mat4f const modelTransform{ transform.worldMatrix };

                for(auto const &mesh : staticModel.model->getMeshes()) {
                    renderer->submitMesh(Renderer::MeshInfo{ mesh, staticModel.material, modelTransform });
                }
            }
        });
        //Submit animated meshes
        entityManager->forEach([this, &deltaTime](TransformComponent const &transform, AnimatedModelComponent &animatedModel) {
            if(animatedModel.model.isValid()) {
                mat4f const modelTransform{ transform.worldMatrix };
                auto const matrixPalet{ animatedModel.animator.update(deltaTime) };

                for(auto const &mesh : animatedModel.model->getMeshes()) {
                    renderer->submitMesh(Renderer::MeshInfo{ mesh, animatedModel.material, modelTransform, matrixPalet });
                }
            }
        });

        //Submit directional lights
        vec3f const dirLightPos{ activeCamera != NullEntity ? entityManager->getComponent<TransformComponent>(activeCamera).position : vec3f{ 0.0f } };
        entityManager->forEach([this, &dirLightPos](DirectionalLightComponent &light) {
            float constexpr mapSize{ 30.0f };
            float constexpr nearDist{ -100.0f };//Negative near dist means we don't have to move the light along it's direction vector
            float constexpr farDist{ 100.0f };
            mat4f const shadowProj{ createOrthographicMatrix(-mapSize, mapSize, -mapSize, mapSize, nearDist, farDist) };

            DirectionalLight lightProxy{
                .data = {
                    .direction = light.direction,
                    .ambient   = light.ambientColour,
                    .diffuse   = light.diffuseColour,
                    .specular  = light.specularColour,
                },
                .shadowTransform = shadowProj * lookAt(dirLightPos, dirLightPos + light.direction, vec3f{ 0.0f, 1.0f, 0.0f }),
            };

            renderer->submitLight(lightProxy);
        });
        //Submit point lights
        entityManager->forEach([this](TransformComponent const &transform, PointLightComponent &light) {
            float constexpr farDist{ 100.0f };
            mat4f const shadowProj{ createPerspectiveMatrix(asRadians(90.0f), 1.0f, 0.5f, farDist) };

            vec3f const &position{ transform.position };

            PointLight lightProxy{
                .data = {
                    .position = position,
                    .ambient  = light.ambientColour,
                    .diffuse  = light.diffuseColour,
                    .specular = light.specularColour,
                    .farPlane = farDist,
                },
                .shadowTransforms = {
                    shadowProj * lookAt(position, position + vec3f{ 1.0f, 0.0f, 0.0f }, vec3f{ 0.0f, 1.0f, 0.0f }),
                    shadowProj * lookAt(position, position + vec3f{ -1.0f, 0.0f, 0.0f }, vec3f{ 0.0f, 1.0f, 0.0f }),
                    shadowProj * lookAt(position, position + vec3f{ 0.0f, 1.0f, 0.0f }, vec3f{ 0.0f, 0.0f, -1.0f }),
                    shadowProj * lookAt(position, position + vec3f{ 0.0f, -1.0f, 0.0f }, vec3f{ 0.0f, 0.0f, 1.0f }),
                    shadowProj * lookAt(position, position + vec3f{ 0.0f, 0.0f, 1.0f }, vec3f{ 0.0f, 1.0f, 0.0f }),
                    shadowProj * lookAt(position, position + vec3f{ 0.0f, 0.0f, -1.0f }, vec3f{ 0.0f, 1.0f, 0.0f }),
                }
            };

            renderer->submitLight(lightProxy);
        });
    }
}
