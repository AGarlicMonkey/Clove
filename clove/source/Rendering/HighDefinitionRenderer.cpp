#include "Clove/Rendering/HighDefinitionRenderer.hpp"

#include "Clove/Application.hpp"
#include "Clove/Rendering/Camera.hpp"
#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/RenderGraph/RenderGraph.hpp"
#include "Clove/Rendering/RenderPasses/DirectionalLightPass.hpp"
#include "Clove/Rendering/RenderPasses/ForwardColourPass.hpp"
#include "Clove/Rendering/RenderPasses/PointLightPass.hpp"
#include "Clove/Rendering/RenderPasses/SkinningPass.hpp"
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

extern "C" const char constants[];
extern "C" const size_t constantsLength;

extern "C" const char ui_v[];
extern "C" const size_t ui_vLength;
extern "C" const char widget_p[];
extern "C" const size_t widget_pLength;
extern "C" const char font_p[];
extern "C" const size_t font_pLength;

namespace clove {
    HighDefinitionRenderer::HighDefinitionRenderer(GhaDevice *ghaDevice, std::unique_ptr<RenderTarget> renderTarget)
        : ghaDevice{ ghaDevice }
        , renderTarget{ std::move(renderTarget) }
        , globalCache{ ghaDevice->getGraphicsFactory() } {
        GhaFactory *ghaFactory{ ghaDevice->getGraphicsFactory() };

        size_t const maxImages{ this->renderTarget->getImages().size() };
        maxFramesInFlight = std::max(static_cast<size_t>(1), maxImages - 1);

        skinningFinishedSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        imageAvailableSemaphores.resize(maxFramesInFlight);
        framesInFlight.resize(maxFramesInFlight);
        imagesInFlight.resize(maxImages);

        graphicsQueue = ghaFactory->createGraphicsQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers }).getValue();
        computeQueue  = ghaFactory->createComputeQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers }).getValue();
        transferQueue = ghaFactory->createTransferQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers }).getValue();

        for(size_t i{ 0 }; i < maxFramesInFlight + 1; ++i) {
            frameCaches.emplace_back(ghaFactory, graphicsQueue.get(), computeQueue.get(), transferQueue.get());
        }

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

        //Create the geometry passes this renderer supports
        geometryPasses.push_back(std::make_unique<SkinningPass>());//NOTE: This needs to be first (or early) as it modifies the vertex buffer.
        geometryPasses.push_back(std::make_unique<ForwardColourPass>());
        geometryPasses.push_back(std::make_unique<DirectionalLightPass>());
        geometryPasses.push_back(std::make_unique<PointLightPass>());

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

        currentFrameData.numLights.numDirectional = 0;
        currentFrameData.numLights.numPoint       = 0;

        for(auto &pass : geometryPasses) {
            pass->flushJobs();
        }
    }

    void HighDefinitionRenderer::submitMesh(MeshInfo meshInfo) {
        currentFrameData.meshes.push_back(std::move(meshInfo));
    }

    void HighDefinitionRenderer::submitCamera(mat4f const view, mat4f const projection, vec3f const position) {
        currentFrameData.viewData.view       = view;
        currentFrameData.viewData.projection = projection;

        currentFrameData.viewPosition = position;
    }

    void HighDefinitionRenderer::submitLight(DirectionalLight const &light) {
        uint32_t const lightIndex{ currentFrameData.numLights.numDirectional++ };

        currentFrameData.lights.directionalLights[lightIndex]    = light.data;
        currentFrameData.directionalShadowTransforms[lightIndex] = light.shadowTransform;
    }

    void HighDefinitionRenderer::submitLight(PointLight const &light) {
        uint32_t const lightIndex{ currentFrameData.numLights.numPoint++ };

        currentFrameData.lights.pointLights[lightIndex]    = light.data;
        currentFrameData.pointShadowTransforms[lightIndex] = light.shadowTransforms;
    }

    void HighDefinitionRenderer::submitWidget(std::shared_ptr<GhaImage> widget, mat4f const modelProjection) {
        currentFrameData.widgets.emplace_back(std::move(widget), modelProjection);
    }

    void HighDefinitionRenderer::submitText(std::shared_ptr<GhaImage> text, mat4f const modelProjection) {
        currentFrameData.text.emplace_back(std::move(text), modelProjection);
    }

    void HighDefinitionRenderer::end() {
        framesInFlight[currentFrame]->wait();

        //Aquire the next available image from the render target
        Expected<uint32_t, std::string> const result{ renderTarget->aquireNextImage(imageAvailableSemaphores[currentFrame].get()) };
        if(!result.hasValue()) {
            CLOVE_LOG(CloveRendering, LogLevel::Debug, result.getError());
            return;
        }

        size_t const imageIndex{ result.getValue() };

        //Check if we're already using the image. If so, wait
        if(imagesInFlight[imageIndex] != nullptr) {
            imagesInFlight[imageIndex]->wait();
        }
        imagesInFlight[imageIndex] = framesInFlight[currentFrame].get();

        framesInFlight[currentFrame]->reset();

        size_t const minUboOffsetAlignment{ ghaDevice->getLimits().minUniformBufferOffsetAlignment };

        //Build the render graph
        RenderGraph renderGraph{ frameCaches[imageIndex], globalCache };

        RgImageId renderTargetImage{ renderGraph.createImage(renderTarget->getImages()[imageIndex]) };
        renderGraph.registerGraphOutput(renderTargetImage);

        RgImageId depthTargetImage{ renderGraph.createImage(GhaImage::Type::_2D, GhaImage::Format::D32_SFLOAT, renderTarget->getSize()) };// TODO: This will probably be a manually created image.

        //Shadows
        std::vector<RgBufferId> directionalLightSpaceBuffers{};

        directionalLightSpaceBuffers.reserve(currentFrameData.numLights.numDirectional);
        for(int32_t i{ 0 }; i < currentFrameData.numLights.numDirectional; ++i) {
            RgBufferId lightSpaceBuffer{ renderGraph.createBuffer(sizeof(mat4f)) };
            renderGraph.writeToBuffer(lightSpaceBuffer, &currentFrameData.directionalShadowTransforms[i], 0, sizeof(mat4f));
            directionalLightSpaceBuffers.push_back(lightSpaceBuffer);
        }

        std::vector<RgBufferId> pointLightSpaceBuffers{};
        std::vector<RgBufferId> pointLightBuffers{};
        size_t pointLightBufferSize{ sizeof(vec3f) + sizeof(float) };

        uint32_t constexpr cubeFaces{ 6 };
        pointLightSpaceBuffers.reserve(currentFrameData.numLights.numPoint * cubeFaces);
        pointLightBuffers.reserve(currentFrameData.numLights.numPoint);
        for(int32_t i{ 0 }; i < currentFrameData.numLights.numPoint; ++i) {
            struct {
                vec3f pos{};
                float farPlane{};
            } const lightData{
                .pos      = currentFrameData.lights.pointLights[i].position,
                .farPlane = currentFrameData.lights.pointLights[i].farPlane,
            };
            RgBufferId lightBuffer{ renderGraph.createBuffer(sizeof(lightData)) };
            renderGraph.writeToBuffer(lightBuffer, &lightData, 0, sizeof(lightData));
            pointLightBuffers.push_back(lightBuffer);

            for(int32_t j{ 0 }; j < cubeFaces; ++j) {
                RgBufferId lightSpaceBuffer{ renderGraph.createBuffer(sizeof(mat4f)) };
                renderGraph.writeToBuffer(lightSpaceBuffer, &currentFrameData.pointShadowTransforms[i][j], 0, sizeof(mat4f));
                pointLightSpaceBuffers.push_back(lightSpaceBuffer);
            }
        }

        RgImageId directionalShadowMap{ renderGraph.createImage(GhaImage::Type::_2D, GhaImage::Format::D32_SFLOAT, { shadowMapSize, shadowMapSize }, MAX_LIGHTS) };
        RgImageId pointShadowMap{ renderGraph.createImage(GhaImage::Type::Cube, GhaImage::Format::D32_SFLOAT, { shadowMapSize, shadowMapSize }, MAX_LIGHTS) };
        RgSampler shadowMaplSampler{ renderGraph.createSampler(GhaSampler::Descriptor{
            .minFilter        = GhaSampler::Filter::Linear,
            .magFilter        = GhaSampler::Filter::Linear,
            .addressModeU     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeV     = GhaSampler::AddressMode::ClampToBorder,
            .addressModeW     = GhaSampler::AddressMode::ClampToBorder,
            .enableAnisotropy = false,
        }) };

        //View uniform buffer
        size_t const viewDataSize{ sizeof(currentFrameData.viewData) };
        size_t const viewPositionSize{ sizeof(currentFrameData.viewPosition) };

        size_t const viewDataOffset{ 0 };
        size_t const viewPositionOffset{ viewDataOffset + viewDataSize + (minUboOffsetAlignment - ((viewDataOffset + viewDataSize) % minUboOffsetAlignment)) };

        RgBufferId viewUniformBuffer{ renderGraph.createBuffer(viewPositionOffset + viewPositionSize) };

        renderGraph.writeToBuffer(viewUniformBuffer, &currentFrameData.viewData, viewDataOffset, viewDataSize);
        renderGraph.writeToBuffer(viewUniformBuffer, &currentFrameData.viewPosition, viewPositionOffset, viewPositionSize);

        //Lights uniform buffer
        size_t const numLightsSize{ sizeof(currentFrameData.numLights) };
        size_t const dirShadowTransformsSize{ sizeof(currentFrameData.directionalShadowTransforms) };
        size_t const lightsSize{ sizeof(currentFrameData.lights) };

        size_t const numLightsOffset{ 0 };
        size_t const dirShadowTransformsOffset{ numLightsOffset + numLightsSize + (minUboOffsetAlignment - ((numLightsOffset + numLightsSize) % minUboOffsetAlignment)) };
        size_t const lightsOffset{ dirShadowTransformsOffset + dirShadowTransformsSize + (minUboOffsetAlignment - ((dirShadowTransformsOffset + dirShadowTransformsSize) % minUboOffsetAlignment)) };

        RgBufferId lightsUniformBuffer{ renderGraph.createBuffer(lightsOffset + lightsSize) };

        renderGraph.writeToBuffer(lightsUniformBuffer, &currentFrameData.numLights, numLightsOffset, numLightsSize);
        renderGraph.writeToBuffer(lightsUniformBuffer, &currentFrameData.directionalShadowTransforms, dirShadowTransformsOffset, dirShadowTransformsSize);
        renderGraph.writeToBuffer(lightsUniformBuffer, &currentFrameData.lights, lightsOffset, lightsSize);

        //Job info
        std::vector<GeometryPass::Job> geometryJobs{ currentFrameData.meshes.size() };
        float constexpr anisotropy{ 16.0f };
        for(size_t i{ 0 }; auto const &meshInfo : currentFrameData.meshes) {
            auto const &mesh{ meshInfo.mesh };

            //Uniforms
            RgBufferId modelBuffer{ renderGraph.createBuffer(sizeof(ModelData)) };
            RgBufferId colourBuffer{ renderGraph.createBuffer(sizeof(vec4f)) };

            ModelData const modelData{
                .model                 = meshInfo.transform,
                .inverseTransposeModel = inverse(transpose(meshInfo.transform)),
            };
            vec4f const colourData{ meshInfo.material->getColour() };

            size_t const modelBufferSize{ sizeof(modelData) };
            size_t const colourBufferSize{ sizeof(colourData) };

            renderGraph.writeToBuffer(modelBuffer, &modelData, 0, sizeof(modelData));
            renderGraph.writeToBuffer(colourBuffer, &colourData, 0, sizeof(colourData));

            //Textures
            RgImageId diffuseTexture{ renderGraph.createImage(meshInfo.material->getDiffuseImage()) };
            RgImageId specularTexture{ renderGraph.createImage(meshInfo.material->getSpecularImage()) };
            RgSampler materialSampler{ renderGraph.createSampler(GhaSampler::Descriptor{
                .minFilter        = GhaSampler::Filter::Linear,
                .magFilter        = GhaSampler::Filter::Linear,
                .addressModeU     = GhaSampler::AddressMode::Repeat,
                .addressModeV     = GhaSampler::AddressMode::Repeat,
                .addressModeW     = GhaSampler::AddressMode::Repeat,
                .enableAnisotropy = true,
                .maxAnisotropy    = anisotropy,
            }) };

            //Animation
            size_t const matrixPaletteSize{ sizeof(mat4f) * MAX_JOINTS };
            RgBufferId matrixPalette{ renderGraph.createBuffer(matrixPaletteSize) };
            renderGraph.writeToBuffer(matrixPalette, meshInfo.matrixPalet.data(), 0, matrixPaletteSize);

            geometryJobs[i] = GeometryPass::Job{
                .vertexBuffer      = renderGraph.createBuffer(mesh->getCombinedBuffer(), mesh->getVertexOffset(), mesh->getVertexBufferSize()),
                .indexBuffer       = renderGraph.createBuffer(mesh->getCombinedBuffer(), mesh->getIndexOffset(), mesh->getIndexBufferSize()),
                .vertexCount       = mesh->getVertexCount(),
                .indexCount        = mesh->getIndexCount(),
                .vertexBufferSize  = mesh->getVertexBufferSize(),
                .modelBuffer       = modelBuffer,
                .colourBuffer      = colourBuffer,
                .modelBufferSize   = modelBufferSize,
                .colourBufferSize  = colourBufferSize,
                .diffuseTexture    = diffuseTexture,
                .specularTexture   = specularTexture,
                .materialSampler   = materialSampler,
                .matrixPalette     = matrixPalette,
                .matrixPaletteSize = matrixPaletteSize,
            };

            for(auto &pass : geometryPasses) {
                if(meshInfo.geometryPassIds.contains(pass->getId())) {
                    pass->addJob(&geometryJobs[i]);
                }
            }

            ++i;
        }

        //Execute passes
        GeometryPass::PassData const passData{
            .renderTarget                 = renderTargetImage,
            .depthTarget                  = depthTargetImage,
            .renderTargetSize             = renderTarget->getSize(),
            .directionalShadowMap         = directionalShadowMap,
            .pointShadowMap               = pointShadowMap,
            .shadowMaplSampler            = shadowMaplSampler,
            .directionalLightCount        = currentFrameData.numLights.numDirectional,
            .pointLightCount              = currentFrameData.numLights.numPoint,
            .directionalLightSpaceBuffers = std::move(directionalLightSpaceBuffers),
            .pointLightSpaceBuffers       = std::move(pointLightSpaceBuffers),
            .pointLightBuffers            = std::move(pointLightBuffers),
            .pointLightBufferSize         = pointLightBufferSize,
            .viewUniformBuffer            = viewUniformBuffer,
            .viewDataSize                 = viewDataSize,
            .viewPositionSize             = viewPositionSize,
            .viewDataOffset               = viewDataOffset,
            .viewPositionOffset           = viewPositionOffset,
            .lightsUniformBuffer          = lightsUniformBuffer,
            .numLightsSize                = numLightsSize,
            .dirShadowTransformsSize      = dirShadowTransformsSize,
            .lightsSize                   = lightsSize,
            .numLightsOffset              = numLightsOffset,
            .dirShadowTransformsOffset    = dirShadowTransformsOffset,
            .lightsOffset                 = lightsOffset,
        };

        for(auto &pass : geometryPasses) {
            pass->execute(renderGraph, passData);
        }

        //Execute UI work
        //TODO: Cache instead of making every frame
        std::unordered_map<std::string, std::string> shaderIncludes;
        shaderIncludes["Constants.glsl"] = { constants, constantsLength };

        RgRenderPass::Descriptor uiPassDescriptor{
            .vertexShader     = renderGraph.createShader({ ui_v, ui_vLength }, shaderIncludes, "UI (vertex)", GhaShader::Stage::Vertex),
            .pixelShader      = renderGraph.createShader({ widget_p, widget_pLength }, shaderIncludes, "Widget (pixel)", GhaShader::Stage::Pixel),
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
            .viewportSize  = passData.renderTargetSize,
            .depthTest     = false,
            .depthWrite    = false,
            .renderTargets = {
                RgRenderTargetBinding{
                    .loadOp     = LoadOperation::Clear,
                    .storeOp    = StoreOperation::Store,
                    .clearValue = ColourValue{ 0.0f, 0.0, 0.0f, 1.0f },
                    .imageView  = {
                        .image = passData.renderTarget,
                    },
                },
            },
            .depthStencil = {
                .loadOp     = LoadOperation::Clear,
                .storeOp    = StoreOperation::DontCare,
                .clearValue = DepthStencilValue{ .depth = 1.0f },
                .imageView  = {
                    .image = passData.depthTarget,
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
                                                                .vertexBuffer = uiVertBuffer,
                                                                .indexBuffer  = uiIndexBuffer,
                                                                .shaderUbos   = {
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
            uiPassDescriptor.pixelShader = renderGraph.createShader({ font_p, font_pLength }, shaderIncludes, "Font (pixel)", GhaShader::Stage::Pixel);
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
                                                              .vertexBuffer = uiVertBuffer,
                                                              .indexBuffer  = uiIndexBuffer,
                                                              .shaderUbos   = {
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
                                                              .shaderImages = {
                                                                  RgImageBinding{
                                                                      .slot      = 3,//NOLINT
                                                                      .imageView = {
                                                                          .image = fontImage,
                                                                      },
                                                                  },

                                                              },
                                                              .shaderSamplers = {
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
        renderTarget->present(imageIndex, { renderFinishedSemaphores[currentFrame].get() });

        //Advance to the next frame
        currentFrame = (currentFrame + 1) % maxFramesInFlight;
    }

    vec2ui HighDefinitionRenderer::getRenderTargetSize() const {
        return renderTarget->getSize();
    }

    void HighDefinitionRenderer::resetGraphCaches() {
        ghaDevice->waitForIdleDevice();

        for(auto &cache : frameCaches) {
            cache.clear();
        }
    }
}
