#pragma once

#include "Clove/Rendering/RenderPasses/GeometryPass.hpp"
#include "Clove/Rendering/ShaderBufferTypes.hpp"

#include <Clove/Delegate/DelegateHandle.hpp>
#include <Clove/Graphics/GhaBuffer.hpp>
#include <Clove/Graphics/GhaDescriptorSetLayout.hpp>
#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GraphicsAPI.hpp>
#include <unordered_map>

#include <set>

namespace garlic::clove {
    class RenderTarget;
    class Window;
    class Camera;
    class Sprite;
    class Mesh;
    class Material;
    class RenderTarget;
}

namespace garlic::clove {
    class ForwardRenderer3D {
        //TYPES
    public:
        struct MeshInfo {
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<Material> material;
            mat4f transform;
            std::array<mat4f, MAX_JOINTS> matrixPalet;
        };

    private:
        //Data for an entire frame
        struct FrameData {
            //Frame data that directly translates into a UBO
            struct BufferData {
                //TODO: Get the alignment from vulkan
                alignas(256) ViewData viewData;
                alignas(256) vec3f viewPosition;

                alignas(256) LightDataArray lights;
                alignas(256) DirectionalShadowTransformArray directionalShadowTransforms;

                alignas(256) LightCount numLights;
            } bufferData;

            std::array<std::array<mat4f, 6>, MAX_LIGHTS> pointShadowTransforms;

            std::vector<MeshInfo> meshes;

            std::vector<std::pair<std::shared_ptr<GhaImageView>, mat4f>> widgets;
            std::vector<std::pair<std::shared_ptr<GhaImageView>, mat4f>> text;

            void forEachMesh(std::function<void(Mesh const &, size_t const index)> func) {
                for(size_t index = 0; auto const &meshInfo : meshes) {
                    func(*meshInfo.mesh, index++);
                }
            }
        };

        /**
         * @brief Objects that hold the state / data of each image (in flight)
         */
        struct ImageData {
            std::shared_ptr<GhaGraphicsCommandBuffer> commandBuffer;
            std::shared_ptr<GhaGraphicsCommandBuffer> shadowMapCommandBuffer;
            std::shared_ptr<GhaGraphicsCommandBuffer> cubeShadowMapCommandBuffer;

            std::shared_ptr<GhaBuffer> frameDataBuffer;            /**< Holds data used across all meshes (lighting, camera etc.). */
            std::vector<std::unique_ptr<GhaBuffer>> objectBuffers; /**< Holds the data for each object. */

            std::shared_ptr<GhaDescriptorPool> frameDescriptorPool; /**< Descriptor pool for sets that change per frame. */
            std::shared_ptr<GhaDescriptorPool> meshDescriptorPool;  /**< Descriptor pool for sets that are for a single mesh's material. */
            std::shared_ptr<GhaDescriptorPool> uiDescriptorPool;    /**< Descriptor pool for sets that are for a ui element. */

            std::shared_ptr<GhaDescriptorSet> viewDescriptorSet;
            std::shared_ptr<GhaDescriptorSet> lightingDescriptorSet;
            std::shared_ptr<GhaDescriptorSet> uiDescriptorSet;

            std::array<std::shared_ptr<GhaImage>, MAX_LIGHTS> shadowMaps;
            std::array<std::shared_ptr<GhaImageView>, MAX_LIGHTS> shadowMapViews;
            std::array<std::shared_ptr<GhaFramebuffer>, MAX_LIGHTS> shadowMapFrameBuffers;

            std::array<std::shared_ptr<GhaImage>, MAX_LIGHTS> cubeShadowMaps;
            std::array<std::shared_ptr<GhaImageView>, MAX_LIGHTS> cubeShadowMapViews;                   //Views the whole cube
            std::array<std::array<std::shared_ptr<GhaImageView>, 6>, MAX_LIGHTS> cubeShadowMapFaceViews;//Views each side of the cube. For the frame buffer
            std::array<std::array<std::shared_ptr<GhaFramebuffer>, 6>, MAX_LIGHTS> cubeShadowMapFrameBuffers;
        };

        //VARIABLES
    private:
        static size_t constexpr maxFramesInFlight{ 2 };
        size_t currentFrame{ 0 };//The current frame we're operating on

        std::unordered_map<std::string, std::string> shaderIncludes;

        DelegateHandle renderTargetPropertyChangedBeginHandle;
        DelegateHandle renderTargetPropertyChangedEndHandle;
        std::unique_ptr<RenderTarget> renderTarget;
        std::vector<std::shared_ptr<GhaFramebuffer>> frameBuffers;//TODO: Move inside the ImageData

        //'Square' mesh used to render UI
        std::unique_ptr<Mesh> uiMesh;

        garlic::clove::GhaDevice *graphicsDevice;
        std::shared_ptr<garlic::clove::GhaFactory> graphicsFactory;

        std::shared_ptr<garlic::clove::GhaGraphicsQueue> graphicsQueue;

        std::unordered_map<DescriptorSetSlots, std::shared_ptr<garlic::clove::GhaDescriptorSetLayout>> descriptorSetLayouts;

        //Frame / image data objects
        FrameData currentFrameData;
        std::vector<ImageData> inFlightImageData;

        //Samplers passed along with textures
        std::shared_ptr<garlic::clove::GhaSampler> textureSampler;
        std::shared_ptr<garlic::clove::GhaSampler> uiSampler;
        std::shared_ptr<garlic::clove::GhaSampler> shadowSampler;

        //Geometry passes
        std::unique_ptr<GeometryPass> colourPass;

        //Objects for the final colour render pass
        std::shared_ptr<garlic::clove::GhaRenderPass> renderPass;
        std::shared_ptr<garlic::clove::GhaPipelineObject> widgetPipelineObject;
        std::shared_ptr<garlic::clove::GhaPipelineObject> textPipelineObject;

        std::shared_ptr<garlic::clove::GhaImage> depthImage;
        std::shared_ptr<garlic::clove::GhaImageView> depthImageView;

        //Objects for the shadow map pass
        std::shared_ptr<garlic::clove::GhaRenderPass> shadowMapRenderPass;
        //std::shared_ptr<garlic::clove::GhaPipelineObject> staticMeshShadowMapPipelineObject;
        std::shared_ptr<garlic::clove::GhaPipelineObject> animatedMeshShadowMapPipelineObject;
        //std::shared_ptr<garlic::clove::GhaPipelineObject> staticMeshCubeShadowMapPipelineObject;
        std::shared_ptr<garlic::clove::GhaPipelineObject> animatedMeshCubeShadowMapPipelineObject;

        //Synchronisation obects
        std::array<std::shared_ptr<garlic::clove::GhaSemaphore>, maxFramesInFlight> shadowFinishedSemaphores;
        std::array<std::shared_ptr<garlic::clove::GhaSemaphore>, maxFramesInFlight> cubeShadowFinishedSemaphores;

        //FUNCTIONS
    public:
        ForwardRenderer3D() = delete;
        ForwardRenderer3D(GhaDevice *graphicsDevice, std::unique_ptr<RenderTarget> renderTarget);

        ForwardRenderer3D(ForwardRenderer3D const &other) = delete;
        //ForwardRenderer3D(ForwardRenderer3D&& other) noexcept;

        ForwardRenderer3D &operator=(ForwardRenderer3D const &other) = delete;
        ForwardRenderer3D &operator=(ForwardRenderer3D &&other) noexcept;

        ~ForwardRenderer3D();

        void begin();

        //TODO: Figuring out the best way for a technique to represent multiple passes

        using PassId = size_t;
        void submitMesh(MeshInfo meshInfo, std::set<PassId> geometryPassIds) {
            //TEMP: Assuming we're only handling static meshes
            currentFrameData.meshes.push_back(std::move(meshInfo));
            //TEMP: Static meshes are manually added as jobs
            colourPass->addJob({ currentFrameData.meshes.size() - 1, currentFrameData.meshes.back().mesh });
        }

        /**
         * @brief Submit the active camera the renderer will use.
         */
        void submitCamera(Camera const &camera, vec3f position);

        void submitLight(DirectionalLight const &light);
        void submitLight(PointLight const &light);

        void submitWidget(std::shared_ptr<garlic::clove::GhaImageView> const widget, mat4f const modelProjection);
        void submitText(std::shared_ptr<garlic::clove::GhaImageView> const text, mat4f const modelProjection);

        void end();

    private:
        void cleanupRenderTargetResources();
        void createRenderTargetResources();

        void createRenderpass();
        void createShadowMapRenderpass();

        void createDepthBuffer();

        void createShadowMapPipeline();
        void createCubeShadowMapPipeline();
        void createUiPipeline();

        void createRenderTargetFrameBuffers();

        std::shared_ptr<garlic::clove::GhaDescriptorPool> createDescriptorPool(std::unordered_map<garlic::clove::DescriptorType, uint32_t> const &bindingCount, uint32_t const setCount);
    };
}
