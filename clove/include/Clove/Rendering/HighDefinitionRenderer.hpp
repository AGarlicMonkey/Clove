#pragma once

#include "Clove/Rendering/RenderGraph/RgFrameCache.hpp"
#include "Clove/Rendering/RenderGraph/RgGlobalCache.hpp"
#include "Clove/Rendering/RenderGraph/RgId.hpp"
#include "Clove/Rendering/RenderGraph/RgSampler.hpp"
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
    class RenderGraph;
}

namespace clove {
    class HighDefinitionRenderer : public Renderer {
        //TYPES
    private:
        struct FrameData {
            ViewData viewData;
            vec3f viewPosition;

            uint32_t numDirLights{ 0 };
            uint32_t numPointLights{ 0 };

            std::vector<LightData> lights;
            mat4f directionalShadowTransform{ 1.0f };
            std::vector<std::array<mat4f, 6>> pointShadowTransforms;

            std::vector<MeshInfo> meshes;

            std::vector<std::pair<std::shared_ptr<GhaImage>, mat4f>> widgets;
            std::vector<std::pair<std::shared_ptr<GhaImage>, mat4f>> text;
        };

        struct ViewUniformBufferData {
            RgBufferId buffer;

            size_t viewDataSize;
            size_t viewPositionSize;

            size_t viewDataOffset;
            size_t viewPositionOffset;
        };

        struct LightBuffers {
            RgBufferId lightsBuffer;    /**< Contains the Light data struct for each light in the scene. */
            RgBufferId lightDataBuffer; /**< Contains info about the lights (how many there are etc). */

            size_t lightsSize{};

            size_t dirShadowTransformOffset{};
            size_t dirShadowTransformSize{};

            size_t totalLightOffset{};
            size_t totalLightSize{};
        };

        struct RenderGraphMeshInfo {
            size_t meshIndex{};

            RgBufferId vertexBuffer{};
            RgBufferId indexBuffer{};

            size_t vertexCount{};
            uint32_t indexCount{};

            size_t vertexBufferSize{};

            RgBufferId modelBuffer{};
            RgBufferId colourBuffer{};

            size_t modelBufferSize{};
            size_t colourBufferSize{};

            RgImageId diffuseTexture{};
            RgImageId specularTexture{};
            RgSampler materialSampler{};
        };

        struct RenderGraphShadowMaps {
            RgImageId directionalShadowMap;
            RgImageId pointShadowMap;
        };

        struct LightGrid {
            RgImageId lightGrid;
            RgBufferId lightIndexList;

            size_t lightIndexListSize{ 0 };
        };

        //VARIABLES
    private:
        static inline uint32_t constexpr minShadowMapNum{ 1 };//Require at least a single map to be created so we have something to bind. TODO: Having shader permutations can avoid this.
        
        size_t maxFramesInFlight{};
        size_t currentFrame{ 0 };//The current frame we're operating on

        GhaDevice *ghaDevice{ nullptr };

        std::unique_ptr<RenderTarget> renderTarget;
        DelegateHandle renderTargetPropertyChangedBeginHandle;

        std::vector<RgFrameCache> frameCaches{};
        RgGlobalCache globalCache;

        std::unordered_map<std::string, std::string> shaderIncludes{};

        //'Square' mesh used to render UI
        std::unique_ptr<Mesh> uiMesh;

        FrameData currentFrameData;

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

        void submitLight(DirectionalLight light) override;
        void submitLight(PointLight light) override;

        void submitWidget(std::shared_ptr<GhaImage> widget, mat4f const modelProjection) override;
        void submitText(std::shared_ptr<GhaImage> text, mat4f const modelProjection) override;

        void end() override;

        vec2ui getRenderTargetSize() const override;

    private:
        RgImageId renderSceneDepth(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> const &meshes, ViewUniformBufferData const &viewUniformBuffer);

        LightGrid computeLightGrid(RenderGraph &renderGraph, LightBuffers const &lightBuffers, RgImageId const sceneDepth);
        void skinMeshes(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> &meshes);

        RenderGraphShadowMaps renderShadowDepths(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> const &meshes);

        void renderSene(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> const &meshes, ViewUniformBufferData const &viewUniformBuffer, LightGrid const &lightGrid, LightBuffers const &lightBuffers, RenderGraphShadowMaps const shadowMaps, RgImageId const renderTarget, RgImageId const depthTarget);

        void resetGraphCaches();
    };
}
