#pragma once

#include "Clove/Rendering/RenderGraph/RgFrameCache.hpp"
#include "Clove/Rendering/RenderGraph/RgGlobalCache.hpp"
#include "Clove/Rendering/Renderer.hpp"

#include <Clove/Delegate/DelegateHandle.hpp>
#include <set>
#include <unordered_map>

namespace clove {
    class GhaDevice;
    class RenderTarget;
    class Camera;
    class Sprite;
    class Mesh;
    class Material;
}

namespace clove {
    class HighDefinitionRenderer : public Renderer{
        //TYPES
    private:
        //Data for an entire frame
        struct FrameData {
            ViewData viewData;
            vec3f viewPosition;

            LightDataArray lights;
            DirectionalShadowTransformArray directionalShadowTransforms;

            LightCount numLights;

            std::array<std::array<mat4f, 6>, MAX_LIGHTS> pointShadowTransforms;

            std::vector<MeshInfo> meshes;

            std::vector<std::pair<std::shared_ptr<GhaImage>, mat4f>> widgets;
            std::vector<std::pair<std::shared_ptr<GhaImage>, mat4f>> text;

            void forEachMesh(std::function<void(Mesh const &, size_t const index)> func) {
                for(size_t index = 0; auto const &meshInfo : meshes) {
                    func(*meshInfo.mesh, index++);
                }
            }
        };
        //VARIABLES
    private:
        size_t maxFramesInFlight{};
        size_t currentFrame{ 0 };//The current frame we're operating on

        GhaDevice *ghaDevice{ nullptr };

        std::unique_ptr<RenderTarget> renderTarget;
        DelegateHandle renderTargetPropertyChangedBeginHandle;

        std::unique_ptr<GhaGraphicsQueue> graphicsQueue{};
        std::unique_ptr<GhaComputeQueue> computeQueue{};
        std::unique_ptr<GhaTransferQueue> transferQueue{};

        std::vector<RgFrameCache> frameCaches{};
        RgGlobalCache globalCache;

        //'Square' mesh used to render UI
        std::unique_ptr<Mesh> uiMesh;

        FrameData currentFrameData;

        //Geometry passes.
        std::vector<std::unique_ptr<GeometryPass>> geometryPasses;

        //Synchronisation obects
        std::vector<std::unique_ptr<GhaSemaphore>> skinningFinishedSemaphores;
        std::vector<std::unique_ptr<GhaSemaphore>> renderFinishedSemaphores;
        std::vector<std::unique_ptr<GhaSemaphore>> imageAvailableSemaphores;
        std::vector<std::unique_ptr<GhaFence>> framesInFlight;
        std::vector<GhaFence *> imagesInFlight;

        //FUNCTIONS
    public:
        HighDefinitionRenderer() = delete;
        HighDefinitionRenderer(GhaDevice *ghaDevice, std::unique_ptr<RenderTarget> renderTarget);

        HighDefinitionRenderer(HighDefinitionRenderer const &other) = delete;
        HighDefinitionRenderer(HighDefinitionRenderer &&other) noexcept;

        HighDefinitionRenderer &operator=(HighDefinitionRenderer const &other) = delete;
        HighDefinitionRenderer &operator=(HighDefinitionRenderer &&other) noexcept;

        ~HighDefinitionRenderer();

        void begin() override;

        void submitMesh(MeshInfo meshInfo) override;

        /**
         * @brief Submit the active camera the renderer will use.
         */
        void submitCamera(mat4f const view, mat4f const projection, vec3f const position) override;

        void submitLight(DirectionalLight const &light) override;
        void submitLight(PointLight const &light) override;

        void submitWidget(std::shared_ptr<GhaImage> widget, mat4f const modelProjection) override;
        void submitText(std::shared_ptr<GhaImage> text, mat4f const modelProjection) override;

        void end() override;

        vec2ui getRenderTargetSize() const override;

    private:
        void resetGraphCaches();
    };
}
