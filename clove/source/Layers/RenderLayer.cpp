#include "Clove/Layers/RenderLayer.hpp"

#include "Clove/Application.hpp"
#include "Clove/Components/AnimatedModelComponent.hpp"
#include "Clove/Components/CameraComponent.hpp"
#include "Clove/Components/DirectionalLightComponent.hpp"
#include "Clove/Components/PointLightComponent.hpp"
#include "Clove/Components/StaticModelComponent.hpp"
#include "Clove/Components/TransformComponent.hpp"
#include "Clove/Rendering/ForwardRenderer3D.hpp"
#include "Clove/Rendering/Renderables/Mesh.hpp"

#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Maths/Maths.hpp>

namespace garlic::clove {
    RenderLayer::RenderLayer()
        : Layer("Render")
        , renderer{ Application::get().getRenderer() }
        , entityManager{ Application::get().getEntityManager() } {
    }

    RenderLayer::RenderLayer(RenderLayer &&other) noexcept = default;

    RenderLayer &RenderLayer::operator=(RenderLayer &&other) noexcept = default;

    RenderLayer::~RenderLayer() = default;

    void RenderLayer::onUpdate(DeltaTime const deltaTime) {
        CLOVE_PROFILE_FUNCTION();

        //Transform and submit cameras
        entityManager->forEach([this](TransformComponent const &transform, CameraComponent &camera) {
            vec3f const position{ transform.getPosition(TransformSpace::World) };

            vec3f const camFront{ transform.getForward() };
            vec3f const camUp{ transform.getUp() };

            camera.setView(lookAt(position, position + camFront, camUp));

            renderer->submitCamera(camera.camera, std::move(position));
        });

        //Submit static meshes
        entityManager->forEach([this](TransformComponent const &transform, StaticModelComponent const &staticModel) {
            mat4f const modelTransform{ transform.getTransformationMatrix(TransformSpace::World) };

            for(auto &mesh : staticModel.model.getMeshes()) {
                renderer->submitStaticMesh(ForwardRenderer3D::StaticMeshInfo{ mesh, staticModel.model.getMaterial(), modelTransform });
            }
        });
        //Submit animated meshes
        entityManager->forEach([this, &deltaTime](TransformComponent const &transform, AnimatedModelComponent &animatedModel) {
            mat4f const modelTransform{ transform.getTransformationMatrix(TransformSpace::World) };
            auto const matrixPalet{ animatedModel.model.update(deltaTime) };

            for(auto &mesh : animatedModel.model.getMeshes()) {
                renderer->submitAnimatedMesh(ForwardRenderer3D::AnimatedMeshInfo{ mesh, animatedModel.model.getMaterial(), modelTransform, matrixPalet });
            }
        });

        //Submit directional lights
        entityManager->forEach([this](DirectionalLightComponent &light) {
            light.lightData.shadowTransform = light.shadowProj * lookAt(-normalise(light.lightData.data.direction) * (light.farDist / 2.0f), vec3f{ 0.0f, 0.0f, 0.0f }, vec3f{ 0.0f, 1.0f, 0.0f });

            renderer->submitLight(light.lightData);
        });
        //Submit point lights
        entityManager->forEach([this](TransformComponent const &transform, PointLightComponent &light) {
            vec3f const &position{ transform.getPosition() };

            light.lightData.data.position       = position;
            light.lightData.shadowTransforms[0] = light.shadowProj * lookAt(position, position + vec3f{ 1.0f, 0.0f, 0.0f }, vec3f{ 0.0f, 1.0f, 0.0f });
            light.lightData.shadowTransforms[1] = light.shadowProj * lookAt(position, position + vec3f{ -1.0f, 0.0f, 0.0f }, vec3f{ 0.0f, 1.0f, 0.0f });
            light.lightData.shadowTransforms[2] = light.shadowProj * lookAt(position, position + vec3f{ 0.0f, 1.0f, 0.0f }, vec3f{ 0.0f, 0.0f, -1.0f });
            light.lightData.shadowTransforms[3] = light.shadowProj * lookAt(position, position + vec3f{ 0.0f, -1.0f, 0.0f }, vec3f{ 0.0f, 0.0f, 1.0f });
            light.lightData.shadowTransforms[4] = light.shadowProj * lookAt(position, position + vec3f{ 0.0f, 0.0f, 1.0f }, vec3f{ 0.0f, 1.0f, 0.0f });
            light.lightData.shadowTransforms[5] = light.shadowProj * lookAt(position, position + vec3f{ 0.0f, 0.0f, -1.0f }, vec3f{ 0.0f, 1.0f, 0.0f });

            renderer->submitLight(light.lightData);
        });
    }
}