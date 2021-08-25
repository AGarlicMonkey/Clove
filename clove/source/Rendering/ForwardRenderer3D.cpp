#include "Clove/Rendering/ForwardRenderer3D.hpp"

#include "Clove/Application.hpp"
#include "Clove/Rendering/Camera.hpp"
#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/RenderPasses/DirectionalLightPass.hpp"
#include "Clove/Rendering/RenderPasses/ForwardColourPass.hpp"
#include "Clove/Rendering/RenderPasses/PointLightPass.hpp"
#include "Clove/Rendering/RenderPasses/SkinningPass.hpp"
#include "Clove/Rendering/RenderTarget.hpp"
#include "Clove/Rendering/Renderables/Mesh.hpp"
#include "Clove/Rendering/RenderingHelpers.hpp"
#include "Clove/Rendering/Vertex.hpp"

#include <Clove/Graphics/GhaDescriptorSet.hpp>
#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaImageView.hpp>
#include <Clove/Graphics/Graphics.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Platform/Window.hpp>
#include <algorithm>

extern "C" const char constants[];
extern "C" const size_t constantsLength;

extern "C" const char ui_v[];
extern "C" const size_t ui_vLength;
extern "C" const char widget_p[];
extern "C" const size_t widget_pLength;
extern "C" const char font_p[];
extern "C" const size_t font_pLength;

namespace clove {
    ForwardRenderer3D::ForwardRenderer3D(GhaFactory *graphicsFactory, std::unique_ptr<RenderTarget> renderTarget)
        : renderTarget{ std::move(renderTarget) }
        , globalCache{ graphicsFactory } {
        size_t const maxImages{ this->renderTarget->getImageViews().size() };
        maxFramesInFlight = std::max(static_cast<size_t>(1), maxImages - 1);

        skinningFinishedSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        imageAvailableSemaphores.resize(maxFramesInFlight);
        framesInFlight.resize(maxFramesInFlight);
        imagesInFlight.resize(maxImages);

        shaderIncludes["Constants.glsl"] = { constants, constantsLength };

        graphicsQueue = graphicsFactory->createGraphicsQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers }).getValue();
        computeQueue  = graphicsFactory->createComputeQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers }).getValue();
        transferQueue = graphicsFactory->createTransferQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers }).getValue();

        for(size_t i{ 0 }; i < maxFramesInFlight; ++i){
            frameCaches.emplace_back(graphicsFactory, graphicsQueue.get(), computeQueue.get(), transferQueue.get());
        }

#if 0
        renderTargetPropertyChangedBeginHandle = this->renderTarget->onPropertiesChangedBegin.bind(&ForwardRenderer3D::cleanupRenderTargetResources, this);
        renderTargetPropertyChangedEndHandle   = this->renderTarget->onPropertiesChangedEnd.bind(&ForwardRenderer3D::createRenderTargetResources, this);
#endif

        //Create semaphores for frame synchronisation
        for(auto &skinningFinishedSemaphore : skinningFinishedSemaphores) {
            skinningFinishedSemaphore = *graphicsFactory->createSemaphore();
        }
        for(auto &renderFinishedSemaphore : renderFinishedSemaphores) {
            renderFinishedSemaphore = *graphicsFactory->createSemaphore();
        }
        for(auto &imageAvailableSemaphore : imageAvailableSemaphores) {
            imageAvailableSemaphore = *graphicsFactory->createSemaphore();
        }

        //Create fences to wait for images in flight
        for(auto &fence : framesInFlight) {
            fence = *graphicsFactory->createFence({ true });
        }

#if 0
        std::vector<Vertex> const uiVertices{
            Vertex{
                .position = { 0.0f, 1.0f, 0.0f },
                .texCoord = { 0.0f, 0.0f },
            },
            Vertex{
                .position = { 1.0f, 1.0f, 0.0f },
                .texCoord = { 1.0f, 0.0f },
            },
            Vertex{
                .position = { 0.0f, 0.0f, 0.0f },
                .texCoord = { 0.0f, 1.0f },
            },
            Vertex{
                .position = { 1.0f, 0.0f, 0.0f },
                .texCoord = { 1.0f, 1.0f },
            },
        };

        std::vector<uint16_t> const uiIndices{
            0,
            2,
            3,
            0,
            3,
            1,
        };

        uiMesh = std::make_unique<Mesh>(uiVertices, uiIndices);
#endif
    }

    ForwardRenderer3D::ForwardRenderer3D(ForwardRenderer3D &&other) noexcept = default;

    ForwardRenderer3D &ForwardRenderer3D::operator=(ForwardRenderer3D &&other) noexcept = default;

    ForwardRenderer3D::~ForwardRenderer3D() = default;

    void ForwardRenderer3D::begin() {
    }

    void ForwardRenderer3D::submitMesh(MeshInfo meshInfo, std::set<GeometryPass::Id> const &geometryPassIds) {
        currentFrameData.meshes.push_back(std::move(meshInfo));
#if 0
        for(auto id : geometryPassIds) {
            geometryPasses[id]->addJob({ currentFrameData.meshes.size() - 1, currentFrameData.meshes.back().mesh });
        }
#endif
    }

    void ForwardRenderer3D::submitCamera(Camera const &camera, vec3f position) {
        currentFrameData.viewData.view       = camera.getView();
        currentFrameData.viewData.projection = camera.getProjection();

        currentFrameData.viewPosition = position;
    }

    void ForwardRenderer3D::submitLight(DirectionalLight const &light) {
        uint32_t const lightIndex{ currentFrameData.numLights.numDirectional++ };

        currentFrameData.lights.directionalLights[lightIndex]    = light.data;
        currentFrameData.directionalShadowTransforms[lightIndex] = light.shadowTransform;
    }

    void ForwardRenderer3D::submitLight(PointLight const &light) {
        uint32_t const lightIndex{ currentFrameData.numLights.numPoint++ };

        currentFrameData.lights.pointLights[lightIndex]    = light.data;
        currentFrameData.pointShadowTransforms[lightIndex] = light.shadowTransforms;
    }

    void ForwardRenderer3D::submitWidget(std::shared_ptr<GhaImageView> const &widget, mat4f const modelProjection) {
#if 0
        currentFrameData.widgets.emplace_back(widget, modelProjection);
#endif
    }

    void ForwardRenderer3D::submitText(std::shared_ptr<GhaImageView> const &text, mat4f const modelProjection) {
#if 0
        currentFrameData.text.emplace_back(text, modelProjection);
#endif
    }

    void ForwardRenderer3D::end() {
        framesInFlight[currentFrame]->wait();

        //Aquire the next available image from the render target
        Expected<uint32_t, std::string> const result{ renderTarget->aquireNextImage(imageAvailableSemaphores[currentFrame].get()) };
        if(!result.hasValue()) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Debug, result.getError());
            return;
        }

        size_t const imageIndex{ result.getValue() };

        //Check if we're already using the image. If so, wait
        if(imagesInFlight[imageIndex] != nullptr) {
            imagesInFlight[imageIndex]->wait();
        }
        imagesInFlight[imageIndex] = framesInFlight[currentFrame].get();

        framesInFlight[currentFrame]->reset();

        //TODO: RenderGraph

        //Tell the render target to present the image we just submitted to
        renderTarget->present(imageIndex, { renderFinishedSemaphores[currentFrame].get() });

        //Advance to the next frame
        currentFrame = (currentFrame + 1) % maxFramesInFlight;
    }

#if 0
    void ForwardRenderer3D::cleanupRenderTargetResources() {
        ghaDevice->waitForIdleDevice();

        frameBuffers.clear();

        for(auto &imageData : inFlightImageData) {
            graphicsQueue->freeCommandBuffer(*imageData.commandBuffer);
            graphicsQueue->freeCommandBuffer(*imageData.shadowMapCommandBuffer);
            graphicsQueue->freeCommandBuffer(*imageData.cubeShadowMapCommandBuffer);
            computeQueue->freeCommandBuffer(*imageData.skinningCommandBuffer);
        }
    }

    void ForwardRenderer3D::createRenderTargetResources() {
        createDepthBuffer();
        createRenderTargetFrameBuffers();

        size_t const imageCount{ frameBuffers.size() };

        inFlightImageData.resize(imageCount);

        //Allocate frame scope descriptor pools
        auto viewSetBindingCount{ countDescriptorBindingTypes(*descriptorSetLayouts[DescriptorSetSlots::View]) };
        auto lightingSetBindingCount{ countDescriptorBindingTypes(*descriptorSetLayouts[DescriptorSetSlots::Lighting]) };

        uint32_t constexpr totalSets{ 2 };//Only 2 sets will be allocated from these pools (view + lighting)
        auto bindingCounts = viewSetBindingCount;
        for(auto &&[type, count] : lightingSetBindingCount) {
            bindingCounts[type] += count;
        }

        for(auto &imageData : inFlightImageData) {
            //Create command buffers
            imageData.commandBuffer              = graphicsQueue->allocateCommandBuffer();
            imageData.shadowMapCommandBuffer     = graphicsQueue->allocateCommandBuffer();
            imageData.cubeShadowMapCommandBuffer = graphicsQueue->allocateCommandBuffer();
            imageData.skinningCommandBuffer      = computeQueue->allocateCommandBuffer();

            //Create uniform buffers
            imageData.frameDataBuffer = *ghaFactory->createBuffer(GhaBuffer::Descriptor{
                .size        = sizeof(FrameData),
                .usageFlags  = GhaBuffer::UsageMode::UniformBuffer,
                .sharingMode = SharingMode::Exclusive,
                .memoryType  = MemoryType::SystemMemory,
            });

            //Create the shadow maps
            //Directional
            imageData.shadowMaps     = *ghaFactory->createImage(GhaImage::Descriptor{
                .type        = GhaImage::Type::_2D,
                .usageFlags  = GhaImage::UsageMode::Sampled | GhaImage::UsageMode::DepthStencilAttachment,
                .dimensions  = { shadowMapSize, shadowMapSize },
                .arrayCount  = MAX_LIGHTS,
                .format      = GhaImage::Format::D32_SFLOAT,
                .sharingMode = SharingMode::Exclusive,
            });
            imageData.shadowMapViews = *ghaFactory->createImageView(*imageData.shadowMaps, GhaImageView::Descriptor{
                                                                                               .type       = GhaImageView::Type::_2D,
                                                                                               .layer      = 0,
                                                                                               .layerCount = MAX_LIGHTS,
                                                                                           });

            //Point
            imageData.cubeShadowMaps     = *ghaFactory->createImage(GhaImage::Descriptor{
                .type        = GhaImage::Type::Cube,
                .usageFlags  = GhaImage::UsageMode::Sampled | GhaImage::UsageMode::DepthStencilAttachment,
                .dimensions  = { shadowMapSize, shadowMapSize },
                .arrayCount  = MAX_LIGHTS,
                .format      = GhaImage::Format::D32_SFLOAT,
                .sharingMode = SharingMode::Exclusive,
            });
            imageData.cubeShadowMapViews = *ghaFactory->createImageView(*imageData.cubeShadowMaps, GhaImageView::Descriptor{
                                                                                                       .type       = GhaImageView::Type::Cube,
                                                                                                       .layer      = 0,
                                                                                                       .layerCount = cubeMapLayerCount * MAX_LIGHTS,
                                                                                                   });

            //Create a view for each element in the array / cube map for the frame buffer in the shadow passes
            for(size_t i = 0; i < MAX_LIGHTS; ++i) {
                imageData.shadowMapArrayLayerViews[i] = *ghaFactory->createImageView(*imageData.shadowMaps, GhaImageView::Descriptor{
                                                                                                                .type       = GhaImageView::Type::_2D,
                                                                                                                .layer      = static_cast<uint32_t>(i),
                                                                                                                .layerCount = 1,
                                                                                                            });
                imageData.shadowMapFrameBuffers[i]    = *ghaFactory->createFramebuffer(GhaFramebuffer::Descriptor{
                    .renderPass  = shadowMapRenderPass.get(),
                    .attachments = { imageData.shadowMapArrayLayerViews[i].get() },
                    .width       = shadowMapSize,
                    .height      = shadowMapSize,
                });

                for(size_t j = 0; j < cubeMapLayerCount; ++j) {
                    imageData.cubeShadowMapFaceViews[i][j] = *ghaFactory->createImageView(*imageData.cubeShadowMaps, GhaImageView::Descriptor{
                                                                                                                         .type       = GhaImageView::Type::_2D,
                                                                                                                         .layer      = static_cast<uint32_t>((i * cubeMapLayerCount) + j),
                                                                                                                         .layerCount = 1,
                                                                                                                     });

                    imageData.cubeShadowMapFrameBuffers[i][j] = *ghaFactory->createFramebuffer(GhaFramebuffer::Descriptor{
                        .renderPass  = shadowMapRenderPass.get(),
                        .attachments = { imageData.cubeShadowMapFaceViews[i][j].get() },
                        .width       = shadowMapSize,
                        .height      = shadowMapSize,
                    });
                }
            }

            //Allocate frame scope descriptor Sets
            imageData.frameDescriptorPool = createDescriptorPool(bindingCounts, totalSets);

            imageData.viewDescriptorSet     = imageData.frameDescriptorPool->allocateDescriptorSets(descriptorSetLayouts[DescriptorSetSlots::View].get());
            imageData.lightingDescriptorSet = imageData.frameDescriptorPool->allocateDescriptorSets(descriptorSetLayouts[DescriptorSetSlots::Lighting].get());

            //As we only have one UBO per frame for every GhaDescriptorSet we can map the buffer into them straight away
            imageData.viewDescriptorSet->map(*imageData.frameDataBuffer, offsetof(FrameData::BufferData, viewData), sizeof(currentFrameData.bufferData.viewData), DescriptorType::UniformBuffer, 0);
            imageData.viewDescriptorSet->map(*imageData.frameDataBuffer, offsetof(FrameData::BufferData, viewPosition), sizeof(currentFrameData.bufferData.viewPosition), DescriptorType::UniformBuffer, 1);

            imageData.lightingDescriptorSet->map(*imageData.frameDataBuffer, offsetof(FrameData::BufferData, lights), sizeof(currentFrameData.bufferData.lights), DescriptorType::UniformBuffer, 0);
            imageData.lightingDescriptorSet->map(*imageData.frameDataBuffer, offsetof(FrameData::BufferData, numLights), sizeof(currentFrameData.bufferData.numLights), DescriptorType::UniformBuffer, 1);
            imageData.lightingDescriptorSet->map(*imageData.frameDataBuffer, offsetof(FrameData::BufferData, directionalShadowTransforms), sizeof(currentFrameData.bufferData.directionalShadowTransforms), DescriptorType::UniformBuffer, 2);
            imageData.lightingDescriptorSet->map(*imageData.shadowMapViews, GhaImage::Layout::ShaderReadOnlyOptimal, 3);
            imageData.lightingDescriptorSet->map(*imageData.cubeShadowMapViews, GhaImage::Layout::ShaderReadOnlyOptimal, 4);
            imageData.lightingDescriptorSet->map(*shadowSampler, 5);//NOLINT
        }
    }
#endif
}
