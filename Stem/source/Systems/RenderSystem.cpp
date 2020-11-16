#include "Stem/Systems/RenderSystem.hpp"

#include "Stem/Components/AnimatedModelComponent.hpp"
#include "Stem/Components/CameraComponent.hpp"
#include "Stem/Components/DirectionalLightComponent.hpp"
#include "Stem/Components/PointLightComponent.hpp"
#include "Stem/Components/StaticModelComponent.hpp"
#include "Stem/Components/TransformComponent.hpp"
#include "Stem/Rendering/ForwardRenderer3D.hpp"
#include "Stem/Rendering/Renderables/Mesh.hpp"
#include "Stem/Application.hpp"

#include <Bulb/ECS/World.hpp>

using namespace clv;
using namespace clv::gfx;

namespace garlic::inline stem {
    RenderSystem::RenderSystem()
        : renderer(Application::get().getRenderer()) {
    }

    RenderSystem::RenderSystem(RenderSystem &&other) noexcept = default;

    RenderSystem &RenderSystem::operator=(RenderSystem &&other) noexcept = default;

    RenderSystem::~RenderSystem() = default;

    void RenderSystem::update(blb::ecs::World &world, utl::DeltaTime deltaTime) {
        CLV_PROFILE_FUNCTION();

        //Transform and submit cameras
        for(auto &&[transform, camera] : world.getComponentSets<TransformComponent, CameraComponent>()) {
            mth::vec3f const position{ transform->getPosition(TransformSpace::World) };

            mth::vec3f const camFront{ transform->getForward() };
            mth::vec3f const camUp{ transform->getUp() };

            mth::mat4f const lookAt{ mth::lookAt(position, position + camFront, camUp) };
            camera->setView(lookAt);

            renderer->submitCamera(camera->camera, std::move(position));
        }

        //Submit static meshes
        for(auto &&[transform, staticModel] : world.getComponentSets<TransformComponent, StaticModelComponent>()) {
            mth::mat4f const modelTransform{ transform->getTransformationMatrix(TransformSpace::World) };

            for(auto &mesh : staticModel->model.getMeshes()) {
                renderer->submitStaticMesh(ForwardRenderer3D::StaticMeshInfo{ mesh, staticModel->model.getMaterial(), modelTransform });
            }
        }
        //Submit animated meshes
        for(auto &&[transform, animatedModel] : world.getComponentSets<TransformComponent, AnimatedModelComponent>()) {
            mth::mat4f const modelTransform{ transform->getTransformationMatrix(TransformSpace::World) };
            auto const matrixPalet{ animatedModel->model.update(deltaTime) };

            for(auto &mesh : animatedModel->model.getMeshes()) {
                renderer->submitAnimatedMesh(ForwardRenderer3D::AnimatedMeshInfo{ mesh, animatedModel->model.getMaterial(), modelTransform, matrixPalet });
            }
        }

        //Submit directional lights
        for(auto &&[light] : world.getComponentSets<DirectionalLightComponent>()) {
            light->lightData.shadowTransform = light->shadowProj * mth::lookAt(-mth::normalise(light->lightData.data.direction) * (light->farDist / 2.0f), mth::vec3f{ 0.0f, 0.0f, 0.0f }, mth::vec3f{ 0.0f, 1.0f, 0.0f });

            renderer->submitLight(light->lightData);
        }

        //Submit point lights
        for(auto &&[transform, light] : world.getComponentSets<TransformComponent, PointLightComponent>()) {
            mth::vec3f const &position{ transform->getPosition() };

            light->lightData.data.position       = position;
            light->lightData.shadowTransforms[0] = light->shadowProj * mth::lookAt(position, position + mth::vec3f{ 1.0f, 0.0f, 0.0f }, mth::vec3f{ 0.0f, 1.0f, 0.0f });
            light->lightData.shadowTransforms[1] = light->shadowProj * mth::lookAt(position, position + mth::vec3f{ -1.0f, 0.0f, 0.0f }, mth::vec3f{ 0.0f, 1.0f, 0.0f });
            light->lightData.shadowTransforms[2] = light->shadowProj * mth::lookAt(position, position + mth::vec3f{ 0.0f, 1.0f, 0.0f }, mth::vec3f{ 0.0f, 0.0f, -1.0f });
            light->lightData.shadowTransforms[3] = light->shadowProj * mth::lookAt(position, position + mth::vec3f{ 0.0f, -1.0f, 0.0f }, mth::vec3f{ 0.0f, 0.0f, 1.0f });
            light->lightData.shadowTransforms[4] = light->shadowProj * mth::lookAt(position, position + mth::vec3f{ 0.0f, 0.0f, 1.0f }, mth::vec3f{ 0.0f, 1.0f, 0.0f });
            light->lightData.shadowTransforms[5] = light->shadowProj * mth::lookAt(position, position + mth::vec3f{ 0.0f, 0.0f, -1.0f }, mth::vec3f{ 0.0f, 1.0f, 0.0f });

            renderer->submitLight(light->lightData);
        }
    }
}