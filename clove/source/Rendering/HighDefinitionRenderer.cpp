#include "Clove/Rendering/HighDefinitionRenderer.hpp"

#include "Clove/Application.hpp"
#include "Clove/Rendering/Camera.hpp"
#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/RenderGraph/RenderGraph.hpp"
#include "Clove/Rendering/RenderTarget.hpp"
#include "Clove/Rendering/Renderables/Mesh.hpp"
#include "Clove/Rendering/RenderingHelpers.hpp"
#include "Clove/Rendering/RenderingLog.hpp"
#include "Clove/Rendering/Vertex.hpp"

#include <Clove/Graphics/GhaDescriptorSet.hpp>
#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaImageView.hpp>
#include <Clove/Platform/Window.hpp>
#include <algorithm>

extern "C" const char common[];
extern "C" const size_t commonLength;

extern "C" const char constants[];
extern "C" const size_t constantsLength;

extern "C" const char scenedepthvert[];
extern "C" const size_t scenedepthvertLength;
extern "C" const char scenedepthpixel[];
extern "C" const size_t scenedepthpixelLength;

extern "C" const char computelightgridcompute[];
extern "C" const size_t computelightgridcomputeLength;

extern "C" const char skinningcompute[];
extern "C" const size_t skinningcomputeLength;

extern "C" const char dirshadowdepthsvert[];
extern "C" const size_t dirshadowdepthsvertLength;
extern "C" const char dirshadowdepthspixel[];
extern "C" const size_t dirshadowdepthspixelLength;

extern "C" const char pointshadowdepthsvert[];
extern "C" const size_t pointshadowdepthsvertLength;
extern "C" const char pointshadowdepthspixel[];
extern "C" const size_t pointshadowdepthspixelLength;

extern "C" const char forwardpassvert[];
extern "C" const size_t forwardpassvertLength;
extern "C" const char forwardpasspixel[];
extern "C" const size_t forwardpasspixelLength;

extern "C" const char uivert[];
extern "C" const size_t uivertLength;
extern "C" const char widgetpixel[];
extern "C" const size_t widgetpixelLength;
extern "C" const char fontpixel[];
extern "C" const size_t fontpixelLength;

namespace clove {
    HighDefinitionRenderer::HighDefinitionRenderer(GhaDevice *ghaDevice, std::unique_ptr<RenderTarget> renderTarget)
        : ghaDevice{ ghaDevice }
        , renderTarget{ std::move(renderTarget) }
        , globalCache{ ghaDevice->getGraphicsFactory() } {
        GhaFactory *ghaFactory{ ghaDevice->getGraphicsFactory() };

        size_t const maxImages{ this->renderTarget->getImageCount() };
        maxFramesInFlight = std::max(static_cast<size_t>(1), maxImages - 1);

        skinningFinishedSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        imageAvailableSemaphores.resize(maxFramesInFlight);
        framesInFlight.resize(maxFramesInFlight);
        imagesInFlight.resize(maxImages);

        graphicsQueue     = ghaFactory->createGraphicsQueue().getValue();
        computeQueue      = ghaFactory->createComputeQueue().getValue();
        asyncComputeQueue = ghaFactory->createAsyncComputeQueue().getValue();
        transferQueue     = ghaFactory->createTransferQueue().getValue();

        for(size_t i{ 0 }; i < maxFramesInFlight + 1; ++i) {
            frameCaches.emplace_back(ghaFactory, graphicsQueue.get(), computeQueue.get(), asyncComputeQueue.get(), transferQueue.get());
        }

        shaderIncludes["Common.glsl"]    = { common, commonLength };
        shaderIncludes["Constants.glsl"] = { constants, constantsLength };

        renderTargetPropertyChangedBeginHandle = this->renderTarget->onPropertiesChangedBegin.bind(&HighDefinitionRenderer::resetGraphCaches, this);

        //Create semaphores for frame synchronisation
        for(auto &skinningFinishedSemaphore : skinningFinishedSemaphores) {
            skinningFinishedSemaphore = ghaFactory->createSemaphore().getValue();
        }
        for(auto &renderFinishedSemaphore : renderFinishedSemaphores) {
            renderFinishedSemaphore = ghaFactory->createSemaphore().getValue();
        }
        for(auto &imageAvailableSemaphore : imageAvailableSemaphores) {
            imageAvailableSemaphore = ghaFactory->createSemaphore().getValue();
        }

        //Create fences to wait for images in flight
        for(auto &fence : framesInFlight) {
            fence = ghaFactory->createFence({ true }).getValue();
        }

        std::vector<Vertex> const uiVertices{
            Vertex{
                .position = { 1.0f, 1.0f, 0.0f },
                .texCoord = { 1.0f, 0.0f },
            },
            Vertex{
                .position = { 0.0f, 1.0f, 0.0f },
                .texCoord = { 0.0f, 0.0f },
            },
            Vertex{
                .position = { 1.0f, 0.0f, 0.0f },
                .texCoord = { 1.0f, 1.0f },
            },
            Vertex{
                .position = { 0.0f, 0.0f, 0.0f },
                .texCoord = { 0.0f, 1.0f },
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
    }

    HighDefinitionRenderer::HighDefinitionRenderer(HighDefinitionRenderer &&other) noexcept = default;

    HighDefinitionRenderer &HighDefinitionRenderer::operator=(HighDefinitionRenderer &&other) noexcept = default;

    HighDefinitionRenderer::~HighDefinitionRenderer() {
        //Wait for an idle device before implicitly destructing the render graph caches.
        //This prevents issues when trying to destroy objects while they might still be in use.
        ghaDevice->waitForIdleDevice();
    }

    void HighDefinitionRenderer::begin() {
        currentFrameData.meshes.clear();
        currentFrameData.widgets.clear();
        currentFrameData.text.clear();

        currentFrameData.lights.clear();
        currentFrameData.pointShadowTransforms.clear();
        currentFrameData.directionalShadowTransform = {};
        currentFrameData.numDirLights   = 0;
        currentFrameData.numPointLights = 0;
    }

    void HighDefinitionRenderer::submitMesh(MeshInfo meshInfo) {
        currentFrameData.meshes.push_back(std::move(meshInfo));
    }

    void HighDefinitionRenderer::submitCamera(mat4f const view, mat4f const projection, vec3f const position) {
        currentFrameData.viewData.view       = view;
        currentFrameData.viewData.projection = projection;

        currentFrameData.viewPosition = position;
    }

    void HighDefinitionRenderer::submitLight(DirectionalLight light) {
        CLOVE_ASSERT_MSG(currentFrameData.numDirLights == 0, "Currently only supporting a single directional lights");

        uint32_t const dirLightIndex{ currentFrameData.numDirLights++ };

        light.data.shadowIndex = dirLightIndex;
        light.data.type        = LIGHT_TYPE_DIRECTIONAL;

        currentFrameData.lights.push_back(light.data);
        currentFrameData.directionalShadowTransform = light.shadowTransform;
    }

    void HighDefinitionRenderer::submitLight(PointLight light) {
        uint32_t const pointLightIndex{ currentFrameData.numPointLights++ };

        light.data.shadowIndex = pointLightIndex;
        light.data.type        = LIGHT_TYPE_POINT;

        currentFrameData.lights.push_back(light.data);
        currentFrameData.pointShadowTransforms.push_back(light.shadowTransforms);
    }

    void HighDefinitionRenderer::submitWidget(std::shared_ptr<GhaImage> widget, mat4f const modelProjection) {
        currentFrameData.widgets.emplace_back(std::move(widget), modelProjection);
    }

    void HighDefinitionRenderer::submitText(std::shared_ptr<GhaImage> text, mat4f const modelProjection) {
        currentFrameData.text.emplace_back(std::move(text), modelProjection);
    }

    void HighDefinitionRenderer::end() {
        //Prevent render graph execution if we do not have anything to render.
        //TODO: This can be removed once shader reflection is in.
        if(currentFrameData.meshes.empty()){
            return;
        }

        framesInFlight[currentFrame]->wait();

        //Aquire the next available image from the render target
        Expected<GhaImage *, std::string> const result{ renderTarget->aquireNextImage(imageAvailableSemaphores[currentFrame].get()) };
        if(!result.hasValue()) {
            CLOVE_LOG(CloveRendering, LogLevel::Debug, result.getError());
            return;
        }

        GhaImage *const swapchainImage{ result.getValue() };
        uint32_t const swapchainImageIndex{ renderTarget->getCurrentImageIndex() };

        //Check if we're already using the image. If so, wait
        if(imagesInFlight[swapchainImageIndex] != nullptr) {
            imagesInFlight[swapchainImageIndex]->wait();
        }
        imagesInFlight[swapchainImageIndex] = framesInFlight[currentFrame].get();

        framesInFlight[currentFrame]->reset();

        RenderGraph renderGraph{ frameCaches[swapchainImageIndex], globalCache };

        RgImageId renderTargetImage{ renderGraph.createImage(swapchainImage) };
        renderGraph.registerGraphOutput(renderTargetImage);

        size_t const minUboOffsetAlignment{ ghaDevice->getLimits().minUniformBufferOffsetAlignment };

        //View uniform buffer
        size_t const viewDataSize{ sizeof(currentFrameData.viewData) };
        size_t const viewPositionSize{ sizeof(currentFrameData.viewPosition) };
        size_t const viewPositionOffset{ viewDataSize + (minUboOffsetAlignment - (viewDataSize % minUboOffsetAlignment)) };

        ViewUniformBufferData const viewUniformData{
            .buffer             = renderGraph.createBuffer(viewPositionOffset + viewPositionSize),
            .viewDataSize       = viewDataSize,
            .viewPositionSize   = viewPositionSize,
            .viewDataOffset     = 0,
            .viewPositionOffset = viewPositionOffset,
        };

        renderGraph.writeToBuffer(viewUniformData.buffer, &currentFrameData.viewData, viewUniformData.viewDataOffset, viewUniformData.viewDataSize);
        renderGraph.writeToBuffer(viewUniformData.buffer, &currentFrameData.viewPosition, viewUniformData.viewPositionOffset, viewUniformData.viewPositionSize);

        //Lights uniform buffer
        size_t const lightsSize{ currentFrameData.lights.size() * sizeof(LightData) };

        size_t const dirShadowTransformOffset{ 0 };
        size_t const dirShadowTransformSize{ sizeof(currentFrameData.directionalShadowTransform) };

        size_t const totalLightOffset{ (dirShadowTransformOffset + dirShadowTransformSize) + (minUboOffsetAlignment - ((dirShadowTransformOffset + dirShadowTransformSize) % minUboOffsetAlignment)) };
        size_t const totalLightSize{ sizeof(uint32_t) };

        LightBuffers const lightBuffers{
            .lightsBuffer             = renderGraph.createBuffer(lightsSize),
            .lightDataBuffer          = renderGraph.createBuffer(totalLightOffset + totalLightSize),
            .lightsSize               = lightsSize,
            .dirShadowTransformOffset = dirShadowTransformOffset,
            .dirShadowTransformSize   = dirShadowTransformSize,
            .totalLightOffset         = totalLightOffset,
            .totalLightSize           = totalLightSize,
        };

        size_t const totalLightCount{ currentFrameData.numDirLights + currentFrameData.numPointLights };

        renderGraph.writeToBuffer(lightBuffers.lightsBuffer, currentFrameData.lights.data(), 0, lightsSize);

        renderGraph.writeToBuffer(lightBuffers.lightDataBuffer, &currentFrameData.directionalShadowTransform, dirShadowTransformOffset, dirShadowTransformSize);
        renderGraph.writeToBuffer(lightBuffers.lightDataBuffer, &totalLightCount, totalLightOffset, totalLightSize);

        //Mesh info
        float constexpr anisotropy{ 16.0f };

        std::vector<RenderGraphMeshInfo> renderGraphMeshes{};
        for(size_t i{ 0 }; i < currentFrameData.meshes.size(); ++i) {
            auto const &meshInfo{ currentFrameData.meshes[i] };
            auto const &mesh{ meshInfo.mesh };

            //Uniforms
            RgBufferId modelBuffer{ renderGraph.createBuffer(sizeof(ModelData)) };
            RgBufferId colourBuffer{ renderGraph.createBuffer(sizeof(vec4f)) };

            ModelData const modelData{
                .model                 = meshInfo.transform,
                .inverseTransposeModel = inverse(transpose(meshInfo.transform)),
            };
            vec4f const colourData{ meshInfo.material.getColour() };

            size_t const modelBufferSize{ sizeof(modelData) };
            size_t const colourBufferSize{ sizeof(colourData) };

            renderGraph.writeToBuffer(modelBuffer, &modelData, 0, sizeof(modelData));
            renderGraph.writeToBuffer(colourBuffer, &colourData, 0, sizeof(colourData));

            //Textures
            RgImageId diffuseTexture{ renderGraph.createImage(meshInfo.material.getDiffuseImage()) };
            RgImageId specularTexture{ renderGraph.createImage(meshInfo.material.getSpecularImage()) };
            RgSampler materialSampler{ renderGraph.createSampler(GhaSampler::Descriptor{
                .minFilter        = GhaSampler::Filter::Linear,
                .magFilter        = GhaSampler::Filter::Linear,
                .addressModeU     = GhaSampler::AddressMode::Repeat,
                .addressModeV     = GhaSampler::AddressMode::Repeat,
                .addressModeW     = GhaSampler::AddressMode::Repeat,
                .enableAnisotropy = true,
                .maxAnisotropy    = anisotropy,
            }) };

            renderGraphMeshes.emplace_back(RenderGraphMeshInfo{
                .meshIndex        = i,
                .vertexBuffer     = renderGraph.createBuffer(mesh->getCombinedBuffer(), mesh->getVertexOffset(), mesh->getVertexBufferSize()),
                .indexBuffer      = renderGraph.createBuffer(mesh->getCombinedBuffer(), mesh->getIndexOffset(), mesh->getIndexBufferSize()),
                .vertexCount      = mesh->getVertexCount(),
                .indexCount       = mesh->getIndexCount(),
                .vertexBufferSize = mesh->getVertexBufferSize(),
                .modelBuffer      = modelBuffer,
                .colourBuffer     = colourBuffer,
                .modelBufferSize  = modelBufferSize,
                .colourBufferSize = colourBufferSize,
                .diffuseTexture   = diffuseTexture,
                .specularTexture  = specularTexture,
                .materialSampler  = materialSampler,
            });
        }

        //Execute passes
        skinMeshes(renderGraph, renderGraphMeshes);

        RgImageId const sceneDepth{ renderSceneDepth(renderGraph, renderGraphMeshes, viewUniformData) };
        LightGrid const lightGrid{ computeLightGrid(renderGraph, lightBuffers, sceneDepth) };

        RenderGraphShadowMaps const shadowMaps{ renderShadowDepths(renderGraph, renderGraphMeshes) };

        renderSene(renderGraph, renderGraphMeshes, viewUniformData, lightGrid, lightBuffers, shadowMaps, renderTargetImage, sceneDepth);

        //Execute UI work
        RgRenderPass::Descriptor uiPassDescriptor{
            .vertexShader     = renderGraph.createShader({ uivert, uivertLength }, shaderIncludes, "UI (vertex)", GhaShader::Stage::Vertex),
            .pixelShader      = renderGraph.createShader({ widgetpixel, widgetpixelLength }, shaderIncludes, "Widget (pixel)", GhaShader::Stage::Pixel),
            .vertexInput      = Vertex::getInputBindingDescriptor(),
            .vertexAttributes = {
                VertexAttributeDescriptor{
                    .format = VertexAttributeFormat::R32G32B32_SFLOAT,
                    .offset = offsetof(Vertex, position),
                },
                VertexAttributeDescriptor{
                    .format = VertexAttributeFormat::R32G32_SFLOAT,
                    .offset = offsetof(Vertex, texCoord),
                },
            },
            .viewportSize  = getRenderTargetSize(),
            .depthTest     = false,
            .depthWrite    = false,
            .renderTargets = {
                RgRenderTargetBinding{
                    .loadOp     = LoadOperation::Clear,
                    .storeOp    = StoreOperation::Store,
                    .clearValue = ColourValue{ 0.0f, 0.0, 0.0f, 1.0f },
                    .imageView  = {
                         .image = renderTargetImage,
                    },
                },
            },
            .depthStencil = {
                .loadOp     = LoadOperation::Clear,
                .storeOp    = StoreOperation::DontCare,
                .clearValue = DepthStencilValue{ .depth = 1.0f },
                .imageView  = {
                     .image = sceneDepth,
                },
            },
        };

        RgBufferId uiVertBuffer{ renderGraph.createBuffer(uiMesh->getCombinedBuffer(), uiMesh->getVertexOffset(), uiMesh->getVertexBufferSize()) };
        RgBufferId uiIndexBuffer{ renderGraph.createBuffer(uiMesh->getCombinedBuffer(), uiMesh->getIndexOffset(), uiMesh->getIndexBufferSize()) };

        if(!currentFrameData.widgets.empty()) {
            RgPassId widgetPass{ renderGraph.createRenderPass(uiPassDescriptor) };
            for(size_t index{ 0 }; auto &&[texture, modelProj] : currentFrameData.widgets) {
                vec4f constexpr colour{ 1.0f };//Temp colour

                size_t constexpr modelProjSize{ sizeof(modelProj) };
                size_t constexpr colourSize{ sizeof(colour) };

                RgBufferId modelBuffer{ renderGraph.createBuffer(modelProjSize) };
                RgBufferId colourBuffer{ renderGraph.createBuffer(colourSize) };

                renderGraph.writeToBuffer(modelBuffer, &modelProj, 0, modelProjSize);
                renderGraph.writeToBuffer(modelBuffer, &colour, 0, colourSize);

                renderGraph.addRenderSubmission(widgetPass, RgRenderPass::Submission{
                                                                .vertexBuffer       = uiVertBuffer,
                                                                .indexBuffer        = uiIndexBuffer,
                                                                .readUniformBuffers = {
                                                                    RgBufferBinding{
                                                                        .slot        = 0,//NOLINT
                                                                        .buffer      = modelBuffer,
                                                                        .size        = modelProjSize,
                                                                        .shaderStage = GhaShader::Stage::Vertex,
                                                                    },
                                                                    RgBufferBinding{
                                                                        .slot        = 1,//NOLINT
                                                                        .buffer      = colourBuffer,
                                                                        .size        = colourSize,
                                                                        .shaderStage = GhaShader::Stage::Pixel,
                                                                    },
                                                                },
                                                                .indexCount = uiMesh->getIndexCount(),
                                                            });

                ++index;
            }
        }

        if(!currentFrameData.text.empty()) {
            uiPassDescriptor.pixelShader = renderGraph.createShader({ fontpixel, fontpixelLength }, shaderIncludes, "Font (pixel)", GhaShader::Stage::Pixel);
            RgPassId textPass{ renderGraph.createRenderPass(uiPassDescriptor) };

            for(size_t index{ 0 }; auto &&[texture, modelProj] : currentFrameData.text) {
                vec4f constexpr colour{ 1.0f };//Temp colour

                size_t constexpr modelProjSize{ sizeof(modelProj) };
                size_t constexpr colourSize{ sizeof(colour) };

                RgBufferId modelBuffer{ renderGraph.createBuffer(modelProjSize) };
                RgBufferId colourBuffer{ renderGraph.createBuffer(colourSize) };
                RgImageId fontImage{ renderGraph.createImage(texture.get()) };
                RgSampler fontSampler{ renderGraph.createSampler(GhaSampler::Descriptor{
                    .minFilter        = GhaSampler::Filter::Nearest,
                    .magFilter        = GhaSampler::Filter::Nearest,
                    .addressModeU     = GhaSampler::AddressMode::ClampToBorder,
                    .addressModeV     = GhaSampler::AddressMode::ClampToBorder,
                    .addressModeW     = GhaSampler::AddressMode::ClampToBorder,
                    .enableAnisotropy = false,
                }) };

                renderGraph.writeToBuffer(modelBuffer, &modelProj, 0, modelProjSize);
                renderGraph.writeToBuffer(modelBuffer, &colour, 0, colourSize);

                renderGraph.addRenderSubmission(textPass, RgRenderPass::Submission{
                                                              .vertexBuffer       = uiVertBuffer,
                                                              .indexBuffer        = uiIndexBuffer,
                                                              .readUniformBuffers = {
                                                                  RgBufferBinding{
                                                                      .slot        = 0,//NOLINT
                                                                      .buffer      = modelBuffer,
                                                                      .size        = modelProjSize,
                                                                      .shaderStage = GhaShader::Stage::Vertex,
                                                                  },
                                                                  RgBufferBinding{
                                                                      .slot        = 1,//NOLINT
                                                                      .buffer      = colourBuffer,
                                                                      .size        = colourSize,
                                                                      .shaderStage = GhaShader::Stage::Pixel,
                                                                  },
                                                              },
                                                              .images = {
                                                                  RgImageBinding{
                                                                      .slot      = 3,//NOLINT
                                                                      .imageView = {
                                                                          .image = fontImage,
                                                                      },
                                                                  },

                                                              },
                                                              .samplers = {
                                                                  RgSamplerBinding{
                                                                      .slot    = 2,//NOLINT
                                                                      .sampler = fontSampler,
                                                                  },
                                                              },
                                                              .indexCount = uiMesh->getIndexCount(),
                                                          });

                ++index;
            }
        }

        renderGraph.execute(RenderGraph::ExecutionInfo{
            .waitSemaphores   = { { imageAvailableSemaphores[currentFrame].get(), PipelineStage::ColourAttachmentOutput } },
            .signalSemaphores = { renderFinishedSemaphores[currentFrame].get() },
            .signalFence      = framesInFlight[currentFrame].get(),
        });

        //Tell the render target to present the image we just submitted to
        renderTarget->present({ renderFinishedSemaphores[currentFrame].get() });

        //Advance to the next frame
        currentFrame = (currentFrame + 1) % maxFramesInFlight;
    }

    vec2ui HighDefinitionRenderer::getRenderTargetSize() const {
        return renderTarget->getSize();
    }

    RgImageId HighDefinitionRenderer::renderSceneDepth(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> const &meshes, ViewUniformBufferData const &viewUniformBuffer) {
        vec2ui const textureSize{ renderTarget->getSize() };

        RgImageId const depthTargetImage{ renderGraph.createImage(GhaImage::Type::_2D, GhaImage::Format::D32_SFLOAT, textureSize) };

        RgRenderPass::Descriptor const passDescriptor{
            .vertexShader     = renderGraph.createShader({ scenedepthvert, scenedepthvertLength }, shaderIncludes, "Scene depth (vertex)", GhaShader::Stage::Vertex),
            .pixelShader      = renderGraph.createShader({ scenedepthpixel, scenedepthpixelLength }, shaderIncludes, "Scene depth (pixel)", GhaShader::Stage::Pixel),
            .vertexInput      = Vertex::getInputBindingDescriptor(),
            .vertexAttributes = {
                VertexAttributeDescriptor{
                    .format = VertexAttributeFormat::R32G32B32_SFLOAT,
                    .offset = offsetof(Vertex, position),
                },
            },
            .viewportSize = textureSize,
            .depthStencil = {
                .loadOp     = LoadOperation::Clear,
                .storeOp    = StoreOperation::Store,
                .clearValue = DepthStencilValue{ .depth = 1.0f },
                .imageView  = {
                     .image = depthTargetImage,
                },
            }
        };
        RgPassId const depthPrePass{ renderGraph.createRenderPass(passDescriptor) };

        for(auto const &mesh : meshes) {
            renderGraph.addRenderSubmission(depthPrePass, RgRenderPass::Submission{
                                                              .vertexBuffer       = mesh.vertexBuffer,
                                                              .indexBuffer        = mesh.indexBuffer,
                                                              .readUniformBuffers = {
                                                                  RgBufferBinding{
                                                                      .slot        = 0,//NOLINT
                                                                      .buffer      = mesh.modelBuffer,
                                                                      .size        = mesh.modelBufferSize,
                                                                      .shaderStage = GhaShader::Stage::Vertex,
                                                                  },
                                                                  RgBufferBinding{
                                                                      .slot        = 1,//NOLINT
                                                                      .buffer      = viewUniformBuffer.buffer,
                                                                      .offset      = viewUniformBuffer.viewDataOffset,
                                                                      .size        = viewUniformBuffer.viewDataSize,
                                                                      .shaderStage = GhaShader::Stage::Vertex,
                                                                  },
                                                              },
                                                              .indexCount = mesh.indexCount,
                                                          });
        }

        return depthTargetImage;
    }

    HighDefinitionRenderer::LightGrid HighDefinitionRenderer::computeLightGrid(RenderGraph &renderGraph, LightBuffers const &lightBuffers, RgImageId const sceneDepth) {
        RgSampler const sceneDepthSampler{ renderGraph.createSampler(GhaSampler::Descriptor{
            .minFilter        = GhaSampler::Filter::Nearest,
            .magFilter        = GhaSampler::Filter::Nearest,
            .addressModeU     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeV     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeW     = GhaSampler::AddressMode::ClampToBorder,
            .enableAnisotropy = false,
        }) };

        size_t constexpr gridSize{ 16 };         //How many threads are in the X and Y of a grid
        size_t constexpr maxLightsPerTile{ 256 };//Current total of how many lights a single tile will deal with.

        vec2ui const screenSize{ renderTarget->getSize() };
        vec2ui const lightGridSize{ screenSize.x / gridSize, screenSize.y / gridSize };

        size_t const lightIndexListSize{ maxLightsPerTile * gridSize * gridSize * sizeof(uint32_t) };

        RgImageId const lightGrid{ renderGraph.createImage(GhaImage::Type::_2D, GhaImage::Format::R32G32_UINT, lightGridSize) };
        RgBufferId const lightIndexList{ renderGraph.createBuffer(lightIndexListSize) };

        RgBufferId const lightCountBuffer{ renderGraph.createBuffer(sizeof(uint32_t)) };

        size_t constexpr memberAlignment{ 16 };
        struct {
            alignas(memberAlignment) vec2ui screenDimensions;
            alignas(memberAlignment) mat4f projection;
            alignas(memberAlignment) mat4f view;
        } const frustumData{
            .screenDimensions = screenSize,
            .projection       = currentFrameData.viewData.projection,
            .view             = currentFrameData.viewData.view,
        };

        size_t const frustumDataSize{ sizeof(frustumData) };
        RgBufferId const frustumDataBuffer{ renderGraph.createBuffer(frustumDataSize) };

        renderGraph.writeToBuffer(frustumDataBuffer, &frustumData, 0, frustumDataSize);

        RgComputePass::Descriptor const passDescriptor{
            .shader = renderGraph.createShader({ computelightgridcompute, computelightgridcomputeLength }, shaderIncludes, "Compute light grid (compute)", GhaShader::Stage::Compute),
        };
        RgPassId const lightGridPass{ renderGraph.createComputePass(passDescriptor, RgSyncType::Sync) };

        renderGraph.addComputeSubmission(lightGridPass, RgComputePass::Submission{
                                                            .readUniformBuffers = {
                                                                RgBufferBinding{
                                                                    .slot   = 4,//NOLINT
                                                                    .buffer = frustumDataBuffer,
                                                                    .size   = frustumDataSize,
                                                                },
                                                                RgBufferBinding{
                                                                    .slot   = 5,//NOLINT
                                                                    .buffer = lightBuffers.lightDataBuffer,
                                                                    .offset = lightBuffers.totalLightOffset,
                                                                    .size   = lightBuffers.totalLightSize,
                                                                },
                                                            },
                                                            .readStorageBuffers = {
                                                                RgBufferBinding{
                                                                    .slot   = 6,//NOLINT
                                                                    .buffer = lightBuffers.lightsBuffer,
                                                                    .size   = lightBuffers.lightsSize,
                                                                },
                                                            },
                                                            .writeBuffers = {
                                                                RgBufferBinding{
                                                                    .slot   = 2,//NOLINT
                                                                    .buffer = lightIndexList,
                                                                    .size   = lightIndexListSize,
                                                                },
                                                                RgBufferBinding{
                                                                    .slot   = 3,//NOLINT
                                                                    .buffer = lightCountBuffer,
                                                                    .size   = sizeof(uint32_t),
                                                                },
                                                            },
                                                            .readImages = {
                                                                RgImageBinding{
                                                                    .slot      = 1,//NOLINT
                                                                    .imageView = RgImageView{
                                                                        .image = sceneDepth,
                                                                    },
                                                                },
                                                            },
                                                            .writeImages = {
                                                                RgImageBinding{
                                                                    .slot      = 0,//NOLINT
                                                                    .imageView = RgImageView{
                                                                        .image = lightGrid,
                                                                    },
                                                                },
                                                            },
                                                            .samplers = {
                                                                RgSamplerBinding{
                                                                    .slot    = 7,//NOLINT
                                                                    .sampler = sceneDepthSampler,
                                                                },
                                                            },
                                                            .disptachSize = vec3ui{ lightGridSize.x, lightGridSize.y, 1 },
                                                        });

        return LightGrid{
            .lightGrid          = lightGrid,
            .lightIndexList     = lightIndexList,
            .lightIndexListSize = lightIndexListSize,
        };
    }

    void HighDefinitionRenderer::skinMeshes(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> &meshes) {
        uint32_t constexpr workgroupSize{ 256 };//Workgroup size of each dispatch - see SkinningCompute.glsl

        RgComputePass::Descriptor passDescriptor{
            .shader = renderGraph.createShader({ skinningcompute, skinningcomputeLength }, shaderIncludes, "Mesh skinner (compute)", GhaShader::Stage::Compute),
        };
        RgPassId skinningPass{ renderGraph.createComputePass(passDescriptor, RgSyncType::Sync) };

        for(auto &mesh : meshes) {
            if(!currentFrameData.meshes[mesh.meshIndex].matrixPalet.has_value()) {
                continue;
            }

            size_t const matrixPaletteSize{ sizeof(mat4f) * MAX_JOINTS };
            RgBufferId matrixPalette{ renderGraph.createBuffer(matrixPaletteSize) };
            renderGraph.writeToBuffer(matrixPalette, currentFrameData.meshes[mesh.meshIndex].matrixPalet->data(), 0, matrixPaletteSize);

            RgBufferId vertCountBuffer{ renderGraph.createBuffer(sizeof(uint32_t)) };
            renderGraph.writeToBuffer(vertCountBuffer, &mesh.vertexCount, 0, sizeof(uint32_t));

            RgBufferId skinnedBuffer{ renderGraph.createBuffer(mesh.vertexBufferSize) };

            renderGraph.addComputeSubmission(skinningPass, RgComputePass::Submission{
                                                               .readUniformBuffers = {
                                                                   RgBufferBinding{
                                                                       .slot   = 0,//NOLINT
                                                                       .buffer = matrixPalette,
                                                                       .size   = matrixPaletteSize,
                                                                   },
                                                                   RgBufferBinding{
                                                                       .slot   = 1,//NOLINT
                                                                       .buffer = vertCountBuffer,
                                                                       .size   = sizeof(uint32_t),
                                                                   },
                                                               },
                                                               .readStorageBuffers = {
                                                                   RgBufferBinding{
                                                                       .slot   = 2,//NOLINT
                                                                       .buffer = mesh.vertexBuffer,
                                                                       .size   = mesh.vertexBufferSize,
                                                                   },
                                                               },
                                                               .writeBuffers = {
                                                                   RgBufferBinding{
                                                                       .slot   = 3,//NOLINT
                                                                       .buffer = skinnedBuffer,
                                                                       .size   = mesh.vertexBufferSize,
                                                                   },
                                                               },
                                                               .disptachSize = { (mesh.vertexCount / workgroupSize) + 1, 1, 1 },
                                                           });

            mesh.vertexBuffer = skinnedBuffer;
        }
    }

    HighDefinitionRenderer::RenderGraphShadowMaps HighDefinitionRenderer::renderShadowDepths(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> const &meshes) {
        RgImageId directionalShadowMap{ renderGraph.createImage(GhaImage::Type::_2D, GhaImage::Format::D32_SFLOAT, { shadowMapSize, shadowMapSize }) };
        RgImageId pointShadowMap{ renderGraph.createImage(GhaImage::Type::Cube, GhaImage::Format::D32_SFLOAT, { shadowMapSize, shadowMapSize }, std::max(currentFrameData.numPointLights, minShadowMapNum)) };

        //Directional lights
        for(auto const &light : currentFrameData.lights) {
            if(light.type == LIGHT_TYPE_POINT) {
                continue;
            }

            RgBufferId lightSpaceBuffer{ renderGraph.createBuffer(sizeof(mat4f)) };
            renderGraph.writeToBuffer(lightSpaceBuffer, &currentFrameData.directionalShadowTransform, 0, sizeof(mat4f));

            //NOTE: Need this as a separate thing otherwise there is an internal compiler error. I think it's because of the clearValue variant
            RgRenderPass::Descriptor passDescriptor{
                .vertexShader     = renderGraph.createShader({ dirshadowdepthsvert, dirshadowdepthsvertLength }, shaderIncludes, "Dir shadow depths (vertex)", GhaShader::Stage::Vertex),
                .pixelShader      = renderGraph.createShader({ dirshadowdepthspixel, dirshadowdepthspixelLength }, shaderIncludes, "Dir shadow depths (pixel)", GhaShader::Stage::Pixel),
                .vertexInput      = Vertex::getInputBindingDescriptor(),
                .vertexAttributes = {
                    VertexAttributeDescriptor{
                        .format = VertexAttributeFormat::R32G32B32_SFLOAT,
                        .offset = offsetof(Vertex, position),
                    },
                },
                .viewportSize = { shadowMapSize, shadowMapSize },
                .depthStencil = {
                    .loadOp     = LoadOperation::Clear,
                    .storeOp    = StoreOperation::Store,
                    .clearValue = DepthStencilValue{ .depth = 1.0f },
                    .imageView  = {
                         .image = directionalShadowMap,
                    },
                }
            };
            RgPassId directionalShadowPass{ renderGraph.createRenderPass(passDescriptor) };

            for(auto const &mesh : meshes) {
                renderGraph.addRenderSubmission(directionalShadowPass, RgRenderPass::Submission{
                                                                           .vertexBuffer       = mesh.vertexBuffer,
                                                                           .indexBuffer        = mesh.indexBuffer,
                                                                           .readUniformBuffers = {
                                                                               RgBufferBinding{
                                                                                   .slot        = 0,//NOLINT
                                                                                   .buffer      = mesh.modelBuffer,
                                                                                   .size        = mesh.modelBufferSize,
                                                                                   .shaderStage = GhaShader::Stage::Vertex,
                                                                               },
                                                                               RgBufferBinding{
                                                                                   .slot        = 1,//NOLINT
                                                                                   .buffer      = lightSpaceBuffer,
                                                                                   .size        = sizeof(mat4f),
                                                                                   .shaderStage = GhaShader::Stage::Vertex,
                                                                               },
                                                                           },
                                                                           .indexCount = mesh.indexCount,
                                                                       });
            }
        }

        //Point lights
        uint32_t constexpr cubeFaces{ 6 };
        for(auto const &light : currentFrameData.lights) {
            if(light.type == LIGHT_TYPE_DIRECTIONAL) {
                continue;
            }

            struct {
                vec3f pos{};
                float radius{};
            } const lightData{
                .pos    = light.position,
                .radius = light.radius,
            };
            size_t constexpr lightDataSize{ sizeof(lightData) };

            RgBufferId lightDataBuffer{ renderGraph.createBuffer(lightDataSize) };
            renderGraph.writeToBuffer(lightDataBuffer, &lightData, 0, lightDataSize);

            for(size_t j{ 0 }; j < cubeFaces; ++j) {
                size_t constexpr lightSpaceSize{ sizeof(mat4f) };
                RgBufferId lightSpaceBuffer{ renderGraph.createBuffer(lightSpaceSize) };
                renderGraph.writeToBuffer(lightSpaceBuffer, &currentFrameData.pointShadowTransforms[light.shadowIndex][j], 0, lightSpaceSize);

                RgRenderPass::Descriptor passDescriptor{
                    .vertexShader     = renderGraph.createShader({ pointshadowdepthsvert, pointshadowdepthsvertLength }, shaderIncludes, "Point shadow depths (vertex)", GhaShader::Stage::Vertex),
                    .pixelShader      = renderGraph.createShader({ pointshadowdepthspixel, pointshadowdepthspixelLength }, shaderIncludes, "Point shadow depths (pixel)", GhaShader::Stage::Pixel),
                    .vertexInput      = Vertex::getInputBindingDescriptor(),
                    .vertexAttributes = {
                        VertexAttributeDescriptor{
                            .format = VertexAttributeFormat::R32G32B32_SFLOAT,
                            .offset = offsetof(Vertex, position),
                        },
                    },
                    .viewportSize = { shadowMapSize, shadowMapSize },
                    .depthStencil = {
                        .loadOp     = LoadOperation::Clear,
                        .storeOp    = StoreOperation::Store,
                        .clearValue = DepthStencilValue{ .depth = 1.0f },
                        .imageView  = {
                             .image      = pointShadowMap,
                             .arrayIndex = static_cast<uint32_t>((light.shadowIndex * cubeFaces) + j),
                        },
                    }
                };
                RgPassId pointShadowPass{ renderGraph.createRenderPass(passDescriptor) };

                for(auto const &mesh : meshes) {
                    renderGraph.addRenderSubmission(pointShadowPass, RgRenderPass::Submission{
                                                                         .vertexBuffer       = mesh.vertexBuffer,
                                                                         .indexBuffer        = mesh.indexBuffer,
                                                                         .readUniformBuffers = {
                                                                             RgBufferBinding{
                                                                                 .slot        = 0,//NOLINT
                                                                                 .buffer      = mesh.modelBuffer,
                                                                                 .size        = mesh.modelBufferSize,
                                                                                 .shaderStage = GhaShader::Stage::Vertex,
                                                                             },
                                                                             RgBufferBinding{
                                                                                 .slot        = 1,//NOLINT
                                                                                 .buffer      = lightSpaceBuffer,
                                                                                 .size        = lightSpaceSize,
                                                                                 .shaderStage = GhaShader::Stage::Vertex,
                                                                             },
                                                                             RgBufferBinding{
                                                                                 .slot        = 2,//NOLINT
                                                                                 .buffer      = lightDataBuffer,
                                                                                 .size        = lightDataSize,
                                                                                 .shaderStage = GhaShader::Stage::Pixel,
                                                                             },
                                                                         },
                                                                         .indexCount = mesh.indexCount,
                                                                     });
                }
            }
        }

        return RenderGraphShadowMaps{ .directionalShadowMap = directionalShadowMap, .pointShadowMap = pointShadowMap };
    }

    void HighDefinitionRenderer::renderSene(RenderGraph &renderGraph, std::vector<RenderGraphMeshInfo> const &meshes, ViewUniformBufferData const &viewUniformBuffer, LightGrid const &lightGrid, LightBuffers const &lightBuffers, RenderGraphShadowMaps const shadowMaps, RgImageId const renderTarget, RgImageId const depthTarget) {
        RgSampler const shadowMapSampler{ renderGraph.createSampler(GhaSampler::Descriptor{
            .minFilter        = GhaSampler::Filter::Linear,
            .magFilter        = GhaSampler::Filter::Linear,
            .addressModeU     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeV     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeW     = GhaSampler::AddressMode::ClampToBorder,
            .enableAnisotropy = false,
        }) };

        RgSampler const lightGridSampler{ renderGraph.createSampler(GhaSampler::Descriptor{
            .minFilter        = GhaSampler::Filter::Nearest,
            .magFilter        = GhaSampler::Filter::Nearest,
            .addressModeU     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeV     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeW     = GhaSampler::AddressMode::ClampToBorder,
            .enableAnisotropy = false,
        }) };

        //NOTE: Need this as a separate thing otherwise there is an internal compiler error. I think it's because of the clearValue variant
        RgRenderPass::Descriptor passDescriptor{
            .vertexShader     = renderGraph.createShader({ forwardpassvert, forwardpassvertLength }, shaderIncludes, "Forward+ (vertex)", GhaShader::Stage::Vertex),
            .pixelShader      = renderGraph.createShader({ forwardpasspixel, forwardpasspixelLength }, shaderIncludes, "Forward+ (pixel)", GhaShader::Stage::Pixel),
            .vertexInput      = Vertex::getInputBindingDescriptor(),
            .vertexAttributes = {
                VertexAttributeDescriptor{
                    .format = VertexAttributeFormat::R32G32B32_SFLOAT,
                    .offset = offsetof(Vertex, position),
                },
                VertexAttributeDescriptor{
                    .format = VertexAttributeFormat::R32G32B32_SFLOAT,
                    .offset = offsetof(Vertex, normal),
                },
                VertexAttributeDescriptor{
                    .format = VertexAttributeFormat::R32G32_SFLOAT,
                    .offset = offsetof(Vertex, texCoord),
                },
            },
            .viewportSize  = getRenderTargetSize(),
            //Currently clearing and then rewriting to the depth buffer. This seems wasteful but without doing this
            // there are culling issues likely down to floating point inaccuracy.
            //.depthWrite    = false,
            .renderTargets = {
                RgRenderTargetBinding{
                    .loadOp     = LoadOperation::Clear,
                    .storeOp    = StoreOperation::Store,
                    .clearValue = ColourValue{ 0.0f, 0.0, 0.0f, 1.0f },
                    .imageView  = {
                         .image = renderTarget,
                    },
                },
            },
            .depthStencil = {
                .loadOp    = LoadOperation::Clear,
                .storeOp   = StoreOperation::DontCare,
                .clearValue = DepthStencilValue{.depth = 1.0f},
                .imageView = {
                    .image = depthTarget,
                },
            },
        };
        RgPassId colourPass{ renderGraph.createRenderPass(passDescriptor) };

        for(auto const &mesh : meshes) {
            renderGraph.addRenderSubmission(colourPass, RgRenderPass::Submission{
                                                            .vertexBuffer       = mesh.vertexBuffer,
                                                            .indexBuffer        = mesh.indexBuffer,
                                                            .readUniformBuffers = {
                                                                RgBufferBinding{
                                                                    .slot        = 0,//NOLINT
                                                                    .buffer      = mesh.modelBuffer,
                                                                    .size        = mesh.modelBufferSize,
                                                                    .shaderStage = GhaShader::Stage::Vertex,
                                                                },
                                                                RgBufferBinding{
                                                                    .slot        = 1,//NOLINT
                                                                    .buffer      = viewUniformBuffer.buffer,
                                                                    .offset      = viewUniformBuffer.viewDataOffset,
                                                                    .size        = viewUniformBuffer.viewDataSize,
                                                                    .shaderStage = GhaShader::Stage::Vertex,
                                                                },
                                                                RgBufferBinding{
                                                                    .slot        = 2,//NOLINT
                                                                    .buffer      = lightBuffers.lightDataBuffer,
                                                                    .offset      = lightBuffers.dirShadowTransformOffset,
                                                                    .size        = lightBuffers.dirShadowTransformSize,
                                                                    .shaderStage = GhaShader::Stage::Vertex,
                                                                },
                                                                RgBufferBinding{
                                                                    .slot        = 10,//NOLINT
                                                                    .buffer      = viewUniformBuffer.buffer,
                                                                    .offset      = viewUniformBuffer.viewPositionOffset,
                                                                    .size        = viewUniformBuffer.viewPositionSize,
                                                                    .shaderStage = GhaShader::Stage::Pixel,
                                                                },
                                                                RgBufferBinding{
                                                                    .slot        = 12,//NOLINT
                                                                    .buffer      = mesh.colourBuffer,
                                                                    .size        = mesh.colourBufferSize,
                                                                    .shaderStage = GhaShader::Stage::Pixel,
                                                                },
                                                            },
                                                            .readStorageBuffers = {
                                                                RgBufferBinding{
                                                                    .slot        = 11,//NOLINT
                                                                    .buffer      = lightBuffers.lightsBuffer,
                                                                    .size        = lightBuffers.lightsSize,
                                                                    .shaderStage = GhaShader::Stage::Pixel,
                                                                },
                                                                RgBufferBinding{
                                                                    .slot        = 14,//NOLINT
                                                                    .buffer      = lightGrid.lightIndexList,
                                                                    .size        = lightGrid.lightIndexListSize,
                                                                    .shaderStage = GhaShader::Stage::Pixel,
                                                                },
                                                            },
                                                            .images = {
                                                                RgImageBinding{
                                                                    .slot      = 4,//NOLINT
                                                                    .imageView = {
                                                                        .image = mesh.diffuseTexture,
                                                                    },
                                                                },
                                                                RgImageBinding{
                                                                    .slot      = 5,//NOLINT
                                                                    .imageView = {
                                                                        .image = mesh.specularTexture,
                                                                    },
                                                                },
                                                                RgImageBinding{
                                                                    .slot      = 7,//NOLINT
                                                                    .imageView = {
                                                                        .image = shadowMaps.directionalShadowMap,
                                                                    },
                                                                },
                                                                RgImageBinding{
                                                                    .slot      = 8,//NOLINT
                                                                    .imageView = {
                                                                        .image      = shadowMaps.pointShadowMap,
                                                                        .viewType   = GhaImageView::Type::CubeArray,
                                                                        .arrayCount = std::max(currentFrameData.numPointLights, minShadowMapNum) * static_cast<uint32_t>(cubeMapLayerCount),
                                                                    },
                                                                },
                                                                RgImageBinding{
                                                                    .slot      = 13,//NOLINT
                                                                    .imageView = {
                                                                        .image = lightGrid.lightGrid,
                                                                    },
                                                                },
                                                            },
                                                            .samplers = {
                                                                RgSamplerBinding{
                                                                    .slot    = 6,//NOLINT
                                                                    .sampler = mesh.materialSampler,
                                                                },
                                                                RgSamplerBinding{
                                                                    .slot    = 9,//NOLINT
                                                                    .sampler = shadowMapSampler,
                                                                },
                                                                RgSamplerBinding{
                                                                    .slot    = 15,//NOLINT
                                                                    .sampler = lightGridSampler,
                                                                },
                                                            },
                                                            .indexCount = mesh.indexCount,
                                                        });
        }
    }

    void HighDefinitionRenderer::resetGraphCaches() {
        ghaDevice->waitForIdleDevice();

        for(auto &cache : frameCaches) {
            cache.clear();
        }
    }
}
