#include "Clove/Rendering/RenderGraph/RenderGraph.hpp"

#include "Clove/Rendering/RenderGraph/RgFrameCache.hpp"
#include "Clove/Rendering/RenderGraph/RgGlobalCache.hpp"
#include "Clove/Rendering/Vertex.hpp"

#include <Clove/Graphics/GhaComputeCommandBuffer.hpp>
#include <Clove/Graphics/GhaComputePipelineObject.hpp>
#include <Clove/Graphics/GhaComputeQueue.hpp>
#include <Clove/Graphics/GhaDescriptorSet.hpp>
#include <Clove/Graphics/GhaDescriptorSetLayout.hpp>
#include <Clove/Graphics/GhaGraphicsCommandBuffer.hpp>
#include <Clove/Graphics/GhaGraphicsPipelineObject.hpp>
#include <Clove/Graphics/GhaRenderPass.hpp>
#include <Clove/Graphics/GhaSampler.hpp>
#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CLOVE_RENDER_GRAPH)

namespace clove {
    RenderGraph::RenderGraph(RgFrameCache &frameCache, RgGlobalCache &globalCache)
        : frameCache{ frameCache }
        , globalCache{ globalCache } {
        frameCache.reset();
    }

    RenderGraph::~RenderGraph() = default;

    RgResourceIdType RenderGraph::createBuffer(size_t const bufferSize) {
        RgResourceIdType const bufferId{ nextResourceId++ };
        buffers[bufferId] = std::make_unique<RgBuffer>(bufferId, bufferSize);

        return bufferId;
    }

    RgResourceIdType RenderGraph::createBuffer(GhaBuffer *buffer, size_t const offset, size_t const size) {
        RgResourceIdType const bufferId{ nextResourceId++ };
        buffers[bufferId] = std::make_unique<RgBuffer>(bufferId, buffer, size, offset);

        return bufferId;
    }

    RgResourceIdType RenderGraph::createImage(GhaImage::Type imageType, GhaImage::Format format, vec2ui dimensions, uint32_t const arrayCount) {
        RgResourceIdType const imageId{ nextResourceId++ };
        images[imageId] = std::make_unique<RgImage>(imageId, imageType, format, dimensions, arrayCount);

        return imageId;
    }

    RgResourceIdType RenderGraph::createImage(GhaImage *ghaImage) {
        RgResourceIdType const imageId{ nextResourceId++ };
        images[imageId] = std::make_unique<RgImage>(imageId, ghaImage);

        return imageId;
    }

    RgSampler RenderGraph::createSampler(GhaSampler::Descriptor descriptor) {
        RgResourceIdType const samplerId{ nextResourceId++ };
        samplers[samplerId] = globalCache.createSampler(std::move(descriptor));

        return samplerId;
    }

    RgShader RenderGraph::createShader(std::filesystem::path const &file, GhaShader::Stage shaderStage) {
        RgShader const shaderId{ nextResourceId++ };
        shaders[shaderId] = globalCache.createShader(file, shaderStage);//Allocate straight away as it's usage does not define it's properties

        return shaderId;
    }

    RgShader RenderGraph::createShader(std::string_view source, std::unordered_map<std::string, std::string> includeSources, std::string_view shaderName, GhaShader::Stage shaderStage) {
        RgShader const shaderId{ nextResourceId++ };
        shaders[shaderId] = globalCache.createShader(source, std::move(includeSources), shaderName, shaderStage);//Allocate straight away as it's usage does not define it's properties

        return shaderId;
    }

    RgPassIdType RenderGraph::createRenderPass(RgRenderPass::Descriptor passDescriptor) {
        RgPassIdType const renderPassId{ nextPassId++ };

        for(auto &renderTarget : passDescriptor.renderTargets) {
            RgResourceIdType const imageId{ renderTarget.target };

            auto &image{ images.at(imageId) };
            if(!image->isExternalImage()) {
                image->addImageUsage(GhaImage::UsageMode::ColourAttachment);
            }
            image->addWritePass(renderPassId);
        }

        {
            RgResourceIdType const imageId{ passDescriptor.depthStencil.target };

            auto &image{ images.at(imageId) };
            if(!image->isExternalImage()) {
                image->addImageUsage(GhaImage::UsageMode::DepthStencilAttachment);
            }
            image->addWritePass(renderPassId);
        }

        renderPasses[renderPassId] = std::make_unique<RgRenderPass>(renderPassId, std::move(passDescriptor));

        return renderPassId;
    }

    RgPassIdType RenderGraph::createComputePass(RgComputePass::Descriptor passDescriptor) {
        RgPassIdType const computePassId{ nextPassId++ };

        computePasses[computePassId] = std::make_unique<RgComputePass>(computePassId, std::move(passDescriptor));

        return computePassId;
    }

    void RenderGraph::registerGraphOutput(RgResourceIdType resource) {
        outputResource = resource;
    }

    void RenderGraph::writeToBuffer(RgResourceIdType buffer, void const *data, size_t const offset, size_t const size) {
        RgPassIdType const transferPassId{ nextPassId++ };

        auto &rgBuffer{ buffers.at(buffer) };
        rgBuffer->makeCpuAccessable();
        rgBuffer->addWritePass(transferPassId);

        RgTransferPass::BufferWrite write{
            .bufferId = buffer,
            .data     = std::vector<std::byte>(size),
            .offset   = offset,
            .size     = size,
        };
        memcpy(write.data.data(), data, size);

        transferPasses[transferPassId] = std::make_unique<RgTransferPass>(transferPassId, std::move(write));
    }

    void RenderGraph::addRenderSubmission(RgPassIdType const renderPass, RgRenderPass::Submission submission) {
        auto &pass{ renderPasses.at(renderPass) };

        {
            RgResourceIdType const bufferId{ submission.vertexBuffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer->isExternalBuffer()) {
                buffer->addBufferUsage(GhaBuffer::UsageMode::VertexBuffer);
            }
            buffer->addReadPass(renderPass);
        }

        {
            RgResourceIdType const bufferId{ submission.indexBuffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer->isExternalBuffer()) {
                buffer->addBufferUsage(GhaBuffer::UsageMode::IndexBuffer);
            }
            buffer->addReadPass(renderPass);
        }

        for(auto const &ubo : submission.shaderUbos) {
            RgResourceIdType const bufferId{ ubo.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer->isExternalBuffer()) {
                buffer->addBufferUsage(GhaBuffer::UsageMode::UniformBuffer);
            }
            buffer->addReadPass(renderPass);
        }

        for(auto const &shaderImage : submission.shaderImages) {
            RgResourceIdType const imageId{ shaderImage.image };

            auto &image{ images.at(imageId) };
            if(!image->isExternalImage()) {
                image->addImageUsage(GhaImage::UsageMode::Sampled);
            }
            image->addReadPass(renderPass);
        }

        pass->addSubmission(std::move(submission));
    }

    void RenderGraph::addComputeSubmission(RgPassIdType const computePass, RgComputePass::Submission submission) {
        auto &pass{ computePasses.at(computePass) };

        for(auto const &ubo : submission.readUniformBuffers) {
            RgResourceIdType const bufferId{ ubo.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer->isExternalBuffer()) {
                buffer->addBufferUsage(GhaBuffer::UsageMode::UniformBuffer);
            }
            buffer->addReadPass(computePass);
        }

        for(auto const &sbo : submission.readStorageBuffers) {
            RgResourceIdType const bufferId{ sbo.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer->isExternalBuffer()) {
                buffer->addBufferUsage(GhaBuffer::UsageMode::StorageBuffer);
            }
            buffer->addReadPass(computePass);
        }

        for(auto const &wb : submission.writeBuffers) {
            RgResourceIdType const bufferId{ wb.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer->isExternalBuffer()) {
                buffer->addBufferUsage(GhaBuffer::UsageMode::StorageBuffer);
            }
            buffer->addWritePass(computePass);
        }

        pass->addSubmission(std::move(submission));
    }

    void RenderGraph::execute(ExecutionInfo const &info) {
        CLOVE_ASSERT(outputResource != INVALID_RESOURCE_ID, "No output resource has been specified");

        //Build the array of passes to execute. Only adding passes which will ultimately effect the output resource.
        std::vector<RgPassIdType> executionPasses{};
        buildExecutionPasses(executionPasses, outputResource);

        //Filter out any duplicates and then reverse order so the output pass is at the end.
        {
            std::unordered_set<RgPassIdType> seenPasses{};
            for(auto iter{ executionPasses.begin() }; iter != executionPasses.end();) {
                if(!seenPasses.contains(*iter)) {
                    seenPasses.emplace(*iter);
                    ++iter;
                } else {
                    iter = executionPasses.erase(iter);
                }
            }
        }
        std::reverse(executionPasses.begin(), executionPasses.end());

        //Build a list of dependecies between the passes. These dependecies signify anything that needs a sempahore.
        std::vector<PassDependency> const passDependencies{ buildDependencies(executionPasses) };

        //Graphics GHA objects.
        std::unordered_map<RgPassIdType, GhaRenderPass *> allocatedRenderPasses{};
        std::unordered_map<RgPassIdType, GhaFramebuffer *> allocatedFramebuffers{};
        std::unordered_map<RgPassIdType, GhaGraphicsPipelineObject *> allocatedGraphicsPipelines{};
        std::unordered_map<RgResourceIdType, GhaSampler *> allocatedSamplers{};

        //Compute GHA objects.
        std::unordered_map<RgPassIdType, GhaComputePipelineObject *> allocatedComputePipelines{};

        //Shared GHA objects.
        std::unordered_map<RgPassIdType, GhaDescriptorSetLayout *> descriptorSetLayouts{};
        std::unordered_map<RgPassIdType, std::vector<std::unique_ptr<GhaDescriptorSet>>> allocatedDescriptorSets{};

        std::unordered_map<DescriptorType, uint32_t> totalDescriptorBindingCount{};
        uint32_t totalDescriptorSets{ 0 };

        //Allocate all of the GHA objects required for each pass
        generateRenderPassObjects(executionPasses, allocatedRenderPasses, allocatedFramebuffers, allocatedGraphicsPipelines, allocatedSamplers, descriptorSetLayouts, totalDescriptorBindingCount, totalDescriptorSets);
        generateComputePassObjects(executionPasses, allocatedComputePipelines, descriptorSetLayouts, totalDescriptorBindingCount, totalDescriptorSets);

        //Allocate descriptor sets
        allocatedDescriptorSets = createDescriptorSets(totalDescriptorBindingCount, totalDescriptorSets, allocatedGraphicsPipelines, allocatedComputePipelines);

        //Change sharing modes if objects are used in multiple queue types.
        //NOTE: Only doing buffers for now as images are yet to be supported
        //TODO: It might be better in the future to just insert the correct memory barriers.
        for(auto &&[bufferId, buffer] : buffers) {
            bool isInGraphics{ false };
            bool isInCompute{ false };

            for(RgPassIdType passId : buffer->getReadPasses()) {
                if(renderPasses.contains(passId)) {
                    isInGraphics = true;
                }
                if(computePasses.contains(passId)) {
                    isInCompute = true;
                }
            }

            for(RgPassIdType passId : buffer->getWritePasses()) {
                if(renderPasses.contains(passId)) {
                    isInGraphics = true;
                }
                if(computePasses.contains(passId)) {
                    isInCompute = true;
                }
            }

            if(isInGraphics && isInCompute) {
                buffer->setSharingMode(SharingMode::Concurrent);
            }
        }

        for(RgPassIdType passId : executionPasses) {
            //Construct any synchronisation objects the pass will need
            std::vector<std::pair<GhaSemaphore const *, PipelineStage>> waitSemaphores{};
            std::vector<GhaSemaphore const *> signalSemaphores{};
            GhaFence *signalFence{ nullptr };

            for(auto const &dependency : passDependencies) {
                if(passId == dependency.waitPass) {
                    waitSemaphores.push_back(std::make_pair(dependency.semaphore, dependency.waitStage));
                } else if(passId == dependency.signalPass) {
                    signalSemaphores.push_back(dependency.semaphore);
                }
            }

            if(passId == executionPasses.back()) {
                waitSemaphores.insert(waitSemaphores.end(), info.waitSemaphores.begin(), info.waitSemaphores.end());
                signalSemaphores.insert(signalSemaphores.end(), info.signalSemaphores.begin(), info.signalSemaphores.end());
                signalFence = info.signalFence;
            }

            //Execute the pass
            if(renderPasses.contains(passId)) {
                GhaGraphicsCommandBuffer *graphicsCommandBufffer{ frameCache.allocateGraphicsCommandBuffer() };

                graphicsCommandBufffer->beginRecording(CommandBufferUsage::OneTimeSubmit);
                executeGraphicsPass(passId, *graphicsCommandBufffer, allocatedRenderPasses, allocatedFramebuffers, allocatedGraphicsPipelines, allocatedSamplers, allocatedDescriptorSets);
                graphicsCommandBufffer->endRecording();

                for(RgPassIdType passId : executionPasses) {
                    if(renderPasses.contains(passId)) {
                        continue;
                    }
                }

                frameCache.submit(GraphicsSubmitInfo{
                                      .waitSemaphores   = waitSemaphores,
                                      .commandBuffers   = { graphicsCommandBufffer },
                                      .signalSemaphores = signalSemaphores,
                                  },
                                  signalFence);

                continue;
            }

            if(computePasses.contains(passId)) {
                GhaComputeCommandBuffer *computeCommandBufffer{ frameCache.allocateComputeCommandBuffer() };

                computeCommandBufffer->beginRecording(CommandBufferUsage::OneTimeSubmit);
                executeComputePass(passId, *computeCommandBufffer, allocatedComputePipelines, allocatedDescriptorSets);
                computeCommandBufffer->endRecording();

                frameCache.submit(ComputeSubmitInfo{
                                      .waitSemaphores   = waitSemaphores,
                                      .commandBuffers   = { computeCommandBufffer },
                                      .signalSemaphores = signalSemaphores,
                                  },
                                  signalFence);

                continue;
            }

            if(transferPasses.contains(passId)) {
                RgTransferPass::BufferWrite const &writeOp{ transferPasses.at(passId)->getWriteOperation() };
                std::unique_ptr<RgBuffer> const &buffer{ buffers.at(writeOp.bufferId) };
                buffer->getGhaBuffer(frameCache)->write(writeOp.data.data(), buffer->getBufferOffset() + writeOp.offset, writeOp.size);

                continue;
            }
        }
    }

    void RenderGraph::buildExecutionPasses(std::vector<RgPassIdType> &outPasses, RgResourceIdType resourceId) {
        RgResource *resource{ getResourceFromId(resourceId) };

        std::vector<RgPassIdType> resourceWritePasses{};
        for(RgPassIdType passId : resource->getWritePasses()) {
            resourceWritePasses.push_back(passId);
        }

        outPasses.insert(outPasses.end(), resourceWritePasses.begin(), resourceWritePasses.end());

        for(RgPassIdType passId : resourceWritePasses) {
            RgPass *pass{ getPassFromId(passId) };
            for(RgResourceIdType resource : pass->getInputResources()) {
                buildExecutionPasses(outPasses, resource);
            }
        }
    }

    std::vector<RenderGraph::PassDependency> RenderGraph::buildDependencies(std::vector<RgPassIdType> const &passes) {
        std::vector<RenderGraph::PassDependency> dependencies{};

        auto const indexOf = [&passes](RgPassIdType passId) -> size_t {
            for(size_t i{ 0 }; i < passes.size(); ++i) {
                if(passes[i] == passId) {
                    return i;
                }
            }
            return -1;
        };

        //Dependecies are built for the closest pass that has an output of the current passes inputs. All queue operations complete in order so we only need to wait on the last one
        //First pass - Create dependecies for render pass inputs
        for(size_t passIndex{ 0 }; passIndex < passes.size(); ++passIndex) {
            RgPassIdType const passId{ passes[passIndex] };
            if(renderPasses.contains(passId)) {
                RgRenderPass const *const dependantPass{ renderPasses.at(passId).get() };
                for(RgResourceIdType const resourceId : dependantPass->getInputResources()) {
                    RgResource const *const resource{ getResourceFromId(resourceId) };
                    RgPassIdType dependencyId{ 0 };
                    size_t currentDistance{ -1u };
                    bool hasDependency{ false };

                    for(RgPassIdType const dependencyPassId : resource->getWritePasses()) {
                        //Only check compute passes for dependencies as all render passes will execute on the same queue.
                        if(computePasses.contains(dependencyPassId)) {
                            size_t const dependencyIndex{ indexOf(dependencyPassId) };
                            //Dependecy needs to be before this pass.
                            if(dependencyIndex > passIndex) {
                                continue;
                            }

                            size_t const distance{ passId - dependencyIndex };
                            if(!hasDependency || distance < currentDistance) {
                                dependencyId    = dependencyPassId;
                                currentDistance = distance;
                                hasDependency   = true;
                            }
                        }
                    }

                    if(hasDependency) {
                        bool createDependency{ true };
                        //If a render pass before this one has a dependency then we don't need to wait because all queue submissions are done in order.
                        for(PassDependency const &dependency : dependencies) {
                            if(renderPasses.contains(dependency.waitPass) && indexOf(dependency.waitPass) < passIndex) {
                                createDependency = false;
                                break;
                            }
                        }

                        if(createDependency) {
                            //Get the correct waitstage for the dependency.
                            //If it's an image we can always assume PipelineStage::PixelShader for now because we only check input resources.
                            //If it's a buffer we check the submission info if it'll be PipelineStage::VertexShader or PipelineStage::PixelShader
                            PipelineStage waitStage{ PipelineStage::Top };
                            if(images.contains(resourceId)) {
                                waitStage = PipelineStage::PixelShader;
                                //TODO: Might need to use early / late fragment tests if the image is a depth buffer
                            } else if(buffers.contains(resourceId)) {
                                for(auto const &submission : dependantPass->getSubmissions()) {
                                    if(submission.vertexBuffer == resourceId || submission.indexBuffer == resourceId) {
                                        waitStage = PipelineStage::VertexInput;
                                        break;
                                    }

                                    bool found{ false };
                                    for(auto const &ubo : submission.shaderUbos) {
                                        if(ubo.slot == resourceId) {
                                            if(ubo.shaderStage == GhaShader::Stage::Vertex){
                                                waitStage = PipelineStage::VertexShader;
                                                found     = true;
                                                break;
                                            }else if(ubo.shaderStage == GhaShader::Stage::Pixel){
                                                waitStage = PipelineStage::PixelShader;
                                                found     = true;
                                                break;
                                            }
                                        }
                                    }

                                    if(found) {
                                        break;
                                    }
                                }
                            } else {
                                CLOVE_LOG(LOG_CATEGORY_CLOVE_RENDER_GRAPH, LogLevel::Warning, "{0}: Could not decide waitStage for resource {1}", CLOVE_FUNCTION_NAME_PRETTY, resourceId);
                            }

                            dependencies.emplace_back(PassDependency{
                                .signalPass = dependencyId,
                                .waitPass   = passId,
                                .waitStage  = waitStage,
                                .semaphore  = frameCache.allocateSemaphore(),
                            });
                        }
                    }
                }
            }
        }

        //Second pass - create compute pass dependencies.
        for(size_t passIndex{ 0 }; passIndex < passes.size(); ++passIndex) {
            RgPassIdType const passId{ passes[passIndex] };
            if(computePasses.contains(passId)) {
                for(RgResourceIdType const resourceId : computePasses.at(passId)->getInputResources()) {
                    RgResource const *const resource{ getResourceFromId(resourceId) };
                    RgPassIdType dependencyId{ 0 };
                    size_t currentDistance{ -1u };
                    bool hasDependency{ false };

                    for(RgPassIdType const dependencyPassId : resource->getWritePasses()) {
                        //Only check render passes for dependencies as all render passes will execute on the same queue.
                        if(renderPasses.contains(dependencyPassId)) {
                            size_t const dependencyIndex{ indexOf(dependencyPassId) };
                            //Dependecy needs to be before this pass.
                            if(dependencyIndex > passIndex) {
                                continue;
                            }

                            size_t const distance{ passId - dependencyIndex };
                            if(!hasDependency || distance < currentDistance) {
                                dependencyId    = dependencyPassId;
                                currentDistance = distance;
                                hasDependency   = true;
                            }
                        }

                        //TODO: Will need to check compute when there is both sync / async compute
                    }

                    if(hasDependency) {
                        bool createDependency{ true };
                        //If a render pass before this one has a dependency then we don't need to wait because all queue submissions are done in order.
                        for(PassDependency const &dependency : dependencies) {
                            if(renderPasses.contains(dependency.waitPass) && indexOf(dependency.waitPass) < passIndex) {
                                createDependency = false;
                                break;
                            }
                        }

                        if(createDependency) {
                            dependencies.emplace_back(PassDependency{
                                .signalPass = dependencyId,
                                .waitPass   = passId,
                                .waitStage  = PipelineStage::ComputeShader,
                                .semaphore  = frameCache.allocateSemaphore(),
                            });
                        }
                    }
                }
            }
        }

        return dependencies;
    }

    GhaImage::Layout RenderGraph::getPreviousLayout(std::vector<RgPassIdType> const &passes, int32_t const currentPassIndex, RgResourceIdType const imageId) {
        for(int32_t i = currentPassIndex - 1; i >= 0; --i) {
            if(!renderPasses.contains(passes[i])) {
                continue;//Only evaluate renderpasses for now
            }
            std::unique_ptr<RgRenderPass> const &renderPass{ renderPasses.at(passes[i]) };

            if(renderPass->getInputResources().contains(imageId)) {
                for(auto const &submission : renderPass->getSubmissions()) {
                    for(auto const &image : submission.shaderImages) {
                        if(image.image == imageId) {
                            return GhaImage::Layout::ShaderReadOnlyOptimal;
                        }
                    }
                }

                CLOVE_ASSERT(false, "ImageId is not in any render target's submissions even though it's marked as an input resource");
            } else if(renderPass->getOutputResources().contains(imageId)) {
                for(auto const &renderTarget : renderPass->getDescriptor().renderTargets) {
                    if(renderTarget.target == imageId) {
                        return GhaImage::Layout::ColourAttachmentOptimal;
                    }
                }
                if(renderPass->getDescriptor().depthStencil.target == imageId) {
                    return GhaImage::Layout::ColourAttachmentOptimal;
                }

                CLOVE_ASSERT(false, "ImageId is not in any render target's output even though it's marked as an output resource");
            }
        }

        return GhaImage::Layout::Undefined;
    }

    RgResource *RenderGraph::getResourceFromId(RgResourceIdType resourceId) {
        if(images.contains(resourceId)) {
            return images.at(resourceId).get();
        } else if(buffers.contains(resourceId)) {
            return buffers.at(resourceId).get();
        } else {
            return nullptr;
        }
    }

    RgPass *RenderGraph::getPassFromId(RgPassIdType passId) {
        if(renderPasses.contains(passId)) {
            return renderPasses.at(passId).get();
        } else if(computePasses.contains(passId)) {
            return computePasses.at(passId).get();
        } else if(transferPasses.contains(passId)) {
            return transferPasses.at(passId).get();
        } else {
            CLOVE_ASSERT(false, "Could not find pass ID");
            return nullptr;
        }
    }

    void RenderGraph::generateRenderPassObjects(std::vector<RgPassIdType> const &passes, std::unordered_map<RgPassIdType, GhaRenderPass *> &outRenderPasses, std::unordered_map<RgPassIdType, GhaFramebuffer *> &outFramebuffers, std::unordered_map<RgPassIdType, GhaGraphicsPipelineObject *> &outGraphicsPipelines, std::unordered_map<RgResourceIdType, GhaSampler *> &outSamplers, std::unordered_map<RgPassIdType, GhaDescriptorSetLayout *> &outDescriptorSetLayouts, std::unordered_map<DescriptorType, uint32_t> &totalDescriptorBindingCount, uint32_t &totalDescriptorSets) {
        for(int32_t i{ 0 }; i < passes.size(); ++i) {
            RgPassIdType const passId{ passes[i] };
            if(!renderPasses.contains(passId)) {
                continue;
            }

            RgRenderPass::Descriptor const &passDescriptor{ renderPasses.at(passId)->getDescriptor() };
            std::vector<RgRenderPass::Submission> const &passSubmissions{ renderPasses.at(passId)->getSubmissions() };

            //Build and allocate the render pass
            std::vector<AttachmentDescriptor> colourAttachments{};
            for(auto &renderTarget : passDescriptor.renderTargets) {
                colourAttachments.emplace_back(AttachmentDescriptor{
                    .format         = images[renderTarget.target]->getFormat(),
                    .loadOperation  = renderTarget.loadOp,
                    .storeOperation = renderTarget.storeOp,
                    .initialLayout  = getPreviousLayout(passes, i, renderTarget.target),
                    .usedLayout     = GhaImage::Layout::ColourAttachmentOptimal,
                    .finalLayout    = renderTarget.target == outputResource ? GhaImage::Layout::Present : GhaImage::Layout::ColourAttachmentOptimal,
                });
            }

            RgDepthStencilBinding const &depthStencil{ passDescriptor.depthStencil };
            AttachmentDescriptor depthStencilAttachment{
                .format         = images[depthStencil.target]->getFormat(),
                .loadOperation  = depthStencil.loadOp,
                .storeOperation = depthStencil.storeOp,
                .initialLayout  = getPreviousLayout(passes, i, depthStencil.target),
                .usedLayout     = GhaImage::Layout::DepthStencilAttachmentOptimal,
                .finalLayout    = GhaImage::Layout::DepthStencilAttachmentOptimal,
            };

            outRenderPasses[passId] = globalCache.createRenderPass(GhaRenderPass::Descriptor{ .colourAttachments = std::move(colourAttachments), .depthAttachment = std::move(depthStencilAttachment) });

            //Build descriptor layouts using the first pass.
            //TODO: Get this infomation from shader reflection
            std::vector<DescriptorSetBindingInfo> descriptorBindings{};
            for(auto &ubo : passSubmissions[0].shaderUbos) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = ubo.slot,
                    .type      = DescriptorType::UniformBuffer,
                    .arraySize = 1,
                    .stage     = ubo.shaderStage,//TODO: provided by shader reflection
                });
            }
            for(auto &image : passSubmissions[0].shaderImages) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = image.slot,
                    .type      = DescriptorType::SampledImage,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Pixel,//TODO: provided by shader reflection
                });
            }
            for(auto &sampler : passSubmissions[0].shaderSamplers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sampler.slot,
                    .type      = DescriptorType::Sampler,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Pixel,//TODO: provided shader reflection
                });
            }

            outDescriptorSetLayouts[passId] = globalCache.createDescriptorSetLayout(GhaDescriptorSetLayout::Descriptor{ .bindings = std::move(descriptorBindings) });

            //Build graphics pipeline
            std::vector<VertexAttributeDescriptor> const vertexAttributes{
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
                VertexAttributeDescriptor{
                    .format = VertexAttributeFormat::R32G32B32_SFLOAT,
                    .offset = offsetof(Vertex, colour),
                },
            };

            //TEMP: Using dynamic for now
            AreaDescriptor const viewScissorArea{
                .state = ElementState::Dynamic,
            };

            outGraphicsPipelines[passId] = globalCache.createGraphicsPipelineObject(GhaGraphicsPipelineObject::Descriptor{
                .vertexShader       = shaders.at(passDescriptor.vertexShader),
                .pixelShader        = shaders.at(passDescriptor.pixelShader),
                .vertexInput        = Vertex::getInputBindingDescriptor(),
                .vertexAttributes   = vertexAttributes,
                .viewportDescriptor = viewScissorArea,
                .scissorDescriptor  = viewScissorArea,
                .depthState         = {
                    .depthTest  = passDescriptor.depthTest,
                    .depthWrite = passDescriptor.depthWrite,
                },
                .enableBlending       = passDescriptor.enableBlending,
                .renderPass           = outRenderPasses.at(passId),
                .descriptorSetLayouts = { outDescriptorSetLayouts.at(passId) },
                .pushConstants        = {},
            });

            //Allocate the frame buffer
            std::vector<GhaImageView const *> attachments{};
            for(auto &renderTarget : passDescriptor.renderTargets) {
                attachments.push_back(images.at(renderTarget.target)->createGhaImageView(frameCache, renderTarget.imageArrayIndex, renderTarget.imageArrayCount));
            }
            attachments.push_back(images.at(passDescriptor.depthStencil.target)->createGhaImageView(frameCache, passDescriptor.depthStencil.imageArrayIndex, passDescriptor.depthStencil.imageArrayCount));

            outFramebuffers[passId] = frameCache.allocateFramebuffer(GhaFramebuffer::Descriptor{
                .renderPass  = outRenderPasses.at(passId),
                .attachments = std::move(attachments),
                .width       = images[passDescriptor.renderTargets[0].target]->getDimensions().x,//TEMP: Just using the first target as the size. This will need to be validated
                .height      = images[passDescriptor.renderTargets[0].target]->getDimensions().y,
            });

            //Count descriptor sets required for the entire pass
            for(auto const &submission : passSubmissions) {
                bool const hasUbo{ !submission.shaderUbos.empty() };
                bool const hasImage{ !submission.shaderImages.empty() };
                bool const hasImageSampler{ !submission.shaderSamplers.empty() };

                if(hasUbo) {
                    totalDescriptorBindingCount[DescriptorType::UniformBuffer] += submission.shaderUbos.size();
                }
                if(hasImage) {
                    totalDescriptorBindingCount[DescriptorType::SampledImage] += submission.shaderImages.size();
                }
                if(hasImageSampler) {
                    totalDescriptorBindingCount[DescriptorType::Sampler] += submission.shaderSamplers.size();
                }

                if(hasUbo || hasImageSampler) {
                    ++totalDescriptorSets;//Allocating a single set per submission
                }
            }
        }
    }

    void RenderGraph::generateComputePassObjects(std::vector<RgPassIdType> const &passes, std::unordered_map<RgPassIdType, GhaComputePipelineObject *> &outComputePipelines, std::unordered_map<RgPassIdType, GhaDescriptorSetLayout *> &outDescriptorSetLayouts, std::unordered_map<DescriptorType, uint32_t> &totalDescriptorBindingCount, uint32_t &totalDescriptorSets) {
        for(int32_t i{ 0 }; i < passes.size(); ++i) {
            RgPassIdType const passId{ passes[i] };
            if(!computePasses.contains(passId)) {
                continue;
            }

            RgComputePass::Descriptor const &passDescriptor{ computePasses.at(passId)->getDescriptor() };
            std::vector<RgComputePass::Submission> const &passSubmissions{ computePasses.at(passId)->getSubmissions() };

            //Build descriptor layouts using the first pass.
            //TODO: Get this infomation from shader reflection
            std::vector<DescriptorSetBindingInfo> descriptorBindings{};
            for(auto &ubo : passSubmissions[0].readUniformBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = ubo.slot,
                    .type      = DescriptorType::UniformBuffer,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }
            for(auto &sbo : passSubmissions[0].readStorageBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sbo.slot,
                    .type      = DescriptorType::StorageBuffer,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }
            for(auto &sbo : passSubmissions[0].writeBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sbo.slot,
                    .type      = DescriptorType::StorageBuffer,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }

            outDescriptorSetLayouts[passId] = globalCache.createDescriptorSetLayout(GhaDescriptorSetLayout::Descriptor{ .bindings = std::move(descriptorBindings) });

            //Build compute pipeline
            outComputePipelines[passId] = globalCache.createComputePipelineObject(GhaComputePipelineObject::Descriptor{
                .shader               = shaders.at(passDescriptor.shader),
                .descriptorSetLayouts = { outDescriptorSetLayouts.at(passId) },
                .pushConstants        = {},
            });

            //Count descriptor sets required for the entire pass
            for(auto const &submission : passSubmissions) {
                bool const hasUbo{ !submission.readUniformBuffers.empty() };
                bool const hasSbo{ !submission.readStorageBuffers.empty() };
                bool const hasWriteBuffer{ !submission.writeBuffers.empty() };

                if(hasUbo) {
                    totalDescriptorBindingCount[DescriptorType::UniformBuffer] += submission.readUniformBuffers.size();
                }
                if(hasSbo) {
                    totalDescriptorBindingCount[DescriptorType::StorageBuffer] += submission.readStorageBuffers.size();
                }
                if(hasWriteBuffer) {
                    totalDescriptorBindingCount[DescriptorType::StorageBuffer] += submission.writeBuffers.size();
                }

                if(hasUbo || hasSbo || hasWriteBuffer) {
                    ++totalDescriptorSets;//Allocating a single set per submission
                }
            }
        }
    }

    std::unordered_map<RgPassIdType, std::vector<std::unique_ptr<GhaDescriptorSet>>> RenderGraph::createDescriptorSets(std::unordered_map<DescriptorType, uint32_t> const &totalDescriptorBindingCount, uint32_t const totalDescriptorSets, std::unordered_map<RgPassIdType, GhaGraphicsPipelineObject *> const &graphicsPipelines, std::unordered_map<RgPassIdType, GhaComputePipelineObject *> &computePipelines) {
        std::unordered_map<RgPassIdType, std::vector<std::unique_ptr<GhaDescriptorSet>>> descriptorSets{};

        //Create a single pool to allocate from
        std::vector<DescriptorInfo> descriptorTypes{};
        descriptorTypes.reserve(totalDescriptorBindingCount.size());

        for(auto &&[type, count] : totalDescriptorBindingCount) {
            descriptorTypes.emplace_back(DescriptorInfo{
                .type  = type,
                .count = count,
            });
        }

        GhaDescriptorPool *descriptorPool{ frameCache.allocateDescriptorPool(GhaDescriptorPool::Descriptor{
            .poolTypes = std::move(descriptorTypes),
            .flag      = GhaDescriptorPool::Flag::None,
            .maxSets   = totalDescriptorSets,
        }) };
        descriptorPool->reset();//Make sure we have a fresh pool. TODO: do this inside frame cache? Or have a function to reset states

        //Create a descriptor set for each pipeline
        for(auto &&[id, pipeline] : graphicsPipelines) {
            std::vector<GhaDescriptorSetLayout const *> layouts(renderPasses.at(id)->getSubmissions().size(), pipeline->getDescriptor().descriptorSetLayouts[0]);//TEMP: Using first index as we know pipelines always have a single descriptor for now
            descriptorSets[id] = descriptorPool->allocateDescriptorSets(layouts);
        }
        for(auto &&[id, pipeline] : computePipelines) {
            std::vector<GhaDescriptorSetLayout const *> layouts(computePasses.at(id)->getSubmissions().size(), pipeline->getDescriptor().descriptorSetLayouts[0]);//TEMP: Using first index as we know pipelines always have a single descriptor for now
            descriptorSets[id] = descriptorPool->allocateDescriptorSets(layouts);
        }

        return descriptorSets;
    }

    void RenderGraph::executeGraphicsPass(RgPassIdType passId, GhaGraphicsCommandBuffer &graphicsCommandBufffer, std::unordered_map<RgPassIdType, GhaRenderPass *> const &allocatedRenderPasses, std::unordered_map<RgPassIdType, GhaFramebuffer *> const &allocatedFramebuffers, std::unordered_map<RgPassIdType, GhaGraphicsPipelineObject *> const &allocatedGraphicsPipelines, std::unordered_map<RgResourceIdType, GhaSampler *> const &allocatedSamplers, std::unordered_map<RgPassIdType, std::vector<std::unique_ptr<GhaDescriptorSet>>> const &allocatedDescriptorSets) {
        RgRenderPass::Descriptor const &passDescriptor{ renderPasses.at(passId)->getDescriptor() };
        std::vector<RgRenderPass::Submission> const &passSubmissions{ renderPasses.at(passId)->getSubmissions() };

        //TODO: Batch operations by render pass and start the pass outside of the operation
        RenderArea renderArea{
            .origin = passDescriptor.viewportPosition,
            .size   = passDescriptor.viewportSize,
        };

        std::vector<ClearValue> clearValues{};
        for(auto &target : passDescriptor.renderTargets) {
            clearValues.push_back(target.clearColour);
        }
        clearValues.push_back(passDescriptor.depthStencil.clearValue);

        graphicsCommandBufffer.beginRenderPass(*allocatedRenderPasses.at(passId), *allocatedFramebuffers.at(passId), renderArea, clearValues);

        //TODO: Only do this if viewport is dynamic
        graphicsCommandBufffer.setViewport(passDescriptor.viewportPosition, passDescriptor.viewportSize);
        graphicsCommandBufffer.setScissor({ 0, 0 }, passDescriptor.viewportSize);

        graphicsCommandBufffer.bindPipelineObject(*allocatedGraphicsPipelines.at(passId));

        for(size_t index{ 0 }; auto const &submission : passSubmissions) {
            std::unique_ptr<GhaDescriptorSet> const &descriptorSet{ allocatedDescriptorSets.at(passId)[index] };

            for(auto const &ubo : submission.shaderUbos) {
                //TODO: Handle different allocations within the same buffer
                std::unique_ptr<RgBuffer> const &buffer{ buffers.at(ubo.buffer) };
                descriptorSet->map(*buffer->getGhaBuffer(frameCache), 0, buffer->getBufferSize(), DescriptorType::UniformBuffer, ubo.slot);
            }
            for(auto const &image : submission.shaderImages) {
                descriptorSet->map(*images.at(image.image)->createGhaImageView(frameCache, image.arrayIndex, image.arrayCount), GhaImage::Layout::ShaderReadOnlyOptimal, image.slot);
            }
            for(auto const &sampler : submission.shaderSamplers) {
                descriptorSet->map(*samplers.at(sampler.sampler), sampler.slot);
            }

            graphicsCommandBufffer.bindDescriptorSet(*descriptorSet, 0);//TODO: Multiple sets / only set sets for a whole pass (i.e. view)

            std::unique_ptr<RgBuffer> const &vertexBuffer{ buffers.at(submission.vertexBuffer) };
            std::unique_ptr<RgBuffer> const &indexBuffer{ buffers.at(submission.indexBuffer) };
            graphicsCommandBufffer.bindVertexBuffer(*vertexBuffer->getGhaBuffer(frameCache), vertexBuffer->getBufferOffset());
            graphicsCommandBufffer.bindIndexBuffer(*indexBuffer->getGhaBuffer(frameCache), indexBuffer->getBufferOffset(), IndexType::Uint16);

            graphicsCommandBufffer.drawIndexed(submission.indexCount);

            ++index;
        }

        graphicsCommandBufffer.endRenderPass();
    }

    void RenderGraph::executeComputePass(RgPassIdType passId, GhaComputeCommandBuffer &computeCommandBufffer, std::unordered_map<RgPassIdType, GhaComputePipelineObject *> const &allocatedComputePipelines, std::unordered_map<RgPassIdType, std::vector<std::unique_ptr<GhaDescriptorSet>>> const &allocatedDescriptorSets) {
        RgComputePass::Descriptor const &passDescriptor{ computePasses.at(passId)->getDescriptor() };
        std::vector<RgComputePass::Submission> const &passSubmissions{ computePasses.at(passId)->getSubmissions() };

        computeCommandBufffer.bindPipelineObject(*allocatedComputePipelines.at(passId));

        for(size_t index{ 0 }; auto const &submission : passSubmissions) {
            std::unique_ptr<GhaDescriptorSet> const &descriptorSet{ allocatedDescriptorSets.at(passId)[index] };

            for(auto const &readUB : submission.readUniformBuffers) {
                std::unique_ptr<RgBuffer> const &buffer{ buffers.at(readUB.buffer) };
                descriptorSet->map(*buffer->getGhaBuffer(frameCache), 0, buffer->getBufferSize(), DescriptorType::UniformBuffer, readUB.slot);
            }
            for(auto const &readSB : submission.readStorageBuffers) {
                std::unique_ptr<RgBuffer> const &buffer{ buffers.at(readSB.buffer) };
                descriptorSet->map(*buffer->getGhaBuffer(frameCache), 0, buffer->getBufferSize(), DescriptorType::StorageBuffer, readSB.slot);
            }
            for(auto const &writeSB : submission.writeBuffers) {
                std::unique_ptr<RgBuffer> const &buffer{ buffers.at(writeSB.buffer) };
                descriptorSet->map(*buffer->getGhaBuffer(frameCache), 0, buffer->getBufferSize(), DescriptorType::StorageBuffer, writeSB.slot);
            }

            computeCommandBufffer.bindDescriptorSet(*descriptorSet, 0);

            computeCommandBufffer.disptach(submission.disptachSize);

            ++index;
        }
    }
}
