#include "Clove/Rendering/RenderGraph/RenderGraph.hpp"

#include "Clove/Rendering/RenderGraph/RenderGraphLog.hpp"
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
#include <unordered_set>

namespace clove {
    namespace {
        bool DoViewsOverlap(RgImageView const &a, RgImageView const &b) {
            if(a.image != b.image) {
                return false;
            }

            uint32_t const aStart{ a.arrayIndex };
            uint32_t const aEnd{ a.arrayIndex + (a.arrayCount - 1) };

            uint32_t const bStart{ b.arrayIndex };
            uint32_t const bEnd{ b.arrayIndex + (b.arrayCount - 1) };

            return ((aStart >= bStart && aStart <= bEnd) ||
                    (aEnd >= bStart && aEnd <= bEnd));
        }

        bool isImageAColourAttachment(RgImageView const &imageView, RgRenderPass const &renderPass) {
            if(renderPass.getOutputResources().contains(imageView.image)) {
                for(auto const &renderTarget : renderPass.getDescriptor().renderTargets) {
                    if(DoViewsOverlap(renderTarget.imageView, imageView)) {
                        return true;
                    }
                }
            }

            return false;
        }

        bool isImageADepthStencilAttachment(RgImageView const &imageView, RgRenderPass const &renderPass) {
            if(renderPass.getOutputResources().contains(imageView.image)) {
                return DoViewsOverlap(renderPass.getDescriptor().depthStencil.imageView, imageView);
            }

            return false;
        }
    }

    RenderGraph::RenderGraph(RgFrameCache &frameCache, RgGlobalCache &globalCache)
        : frameCache{ frameCache }
        , globalCache{ globalCache } {
        frameCache.reset();
    }

    RenderGraph::~RenderGraph() = default;

    RgBufferId RenderGraph::createBuffer(size_t const bufferSize) {
        RgBufferId const bufferId{ nextResourceId++ };
        buffers.emplace(std::make_pair(bufferId, RgBuffer{ bufferSize }));

        return bufferId;
    }

    RgBufferId RenderGraph::createBuffer(GhaBuffer *buffer, size_t const offset, size_t const size) {
        RgBufferId const bufferId{ nextResourceId++ };
        buffers.emplace(std::make_pair(bufferId, RgBuffer{ buffer, offset, size }));

        return bufferId;
    }

    RgImageId RenderGraph::createImage(GhaImage::Type const imageType, GhaImage::Format const format, vec2ui const dimensions, uint32_t const arrayCount) {
        RgImageId const imageId{ nextResourceId++ };
        images.emplace(std::make_pair(imageId, RgImage{ imageType, format, dimensions, arrayCount }));

        return imageId;
    }

    RgImageId RenderGraph::createImage(GhaImage *ghaImage) {
        RgImageId const imageId{ nextResourceId++ };
        images.emplace(std::make_pair(imageId, RgImage{ ghaImage }));

        return imageId;
    }

    RgSampler RenderGraph::createSampler(GhaSampler::Descriptor descriptor) {
        RgResourceId const samplerId{ nextResourceId++ };
        samplers[samplerId] = globalCache.createSampler(descriptor);

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

    RgPassId RenderGraph::createRenderPass(RgRenderPass::Descriptor passDescriptor) {
        RgPassId const renderPassId{ nextPassId++ };

        for(auto const &renderTarget : passDescriptor.renderTargets) {
            RgImageId const imageId{ renderTarget.imageView.image };
            auto &image{ images.at(imageId) };

            if(!image.isExternalImage()) {
                image.addImageUsage(GhaImage::UsageMode::ColourAttachment);
            }
            image.addWritePass(renderPassId);
        }

        if(RgDepthStencilBinding const &depthStencil{ passDescriptor.depthStencil }; depthStencil.imageView.image != INVALID_RESOURCE_ID) {
            auto &image{ images.at(depthStencil.imageView.image) };

            if(!image.isExternalImage()) {
                image.addImageUsage(GhaImage::UsageMode::DepthStencilAttachment);
            }

            if(depthStencil.loadOp == LoadOperation::Load) {
                image.addReadPass(renderPassId);
            }

            if(depthStencil.storeOp == StoreOperation::Store) {
                image.addWritePass(renderPassId);
            }
        }

        renderPasses.emplace(std::make_pair(renderPassId, RgRenderPass{ std::move(passDescriptor) }));

        return renderPassId;
    }

    RgPassId RenderGraph::createComputePass(RgComputePass::Descriptor passDescriptor, RgSyncType syncType) {
        RgPassId const computePassId{ nextPassId++ };

        if(syncType == RgSyncType::Sync) {
            computePasses.emplace(std::make_pair(computePassId, RgComputePass{ passDescriptor }));
        } else {
            asyncComputePasses.emplace(std::make_pair(computePassId, RgComputePass{ passDescriptor }));
        }

        return computePassId;
    }

    void RenderGraph::registerGraphOutput(RgResourceId const resource) {
        CLOVE_ASSERT_MSG(buffers.contains(resource) || images.contains(resource), "RenderGraph output must be either a buffer or image registered to this graph.");

        outputResource = resource;
    }

    void RenderGraph::writeToBuffer(RgResourceId buffer, void const *data, size_t const offset, size_t const size) {
        RgPassId const transferPassId{ nextPassId++ };

        auto &rgBuffer{ buffers.at(buffer) };
        rgBuffer.makeCpuAccessable();
        rgBuffer.addWritePass(transferPassId);

        RgTransferPass::BufferWrite write{
            .bufferId = buffer,
            .data     = std::vector<std::byte>(size),
            .offset   = offset,
            .size     = size,
        };
        memcpy(write.data.data(), data, size);

        transferPasses.emplace(std::make_pair(transferPassId, RgTransferPass{ std::move(write) }));
    }

    void RenderGraph::addRenderSubmission(RgPassId const renderPass, RgRenderPass::Submission submission) {
        auto &pass{ renderPasses.at(renderPass) };

        {
            RgResourceId const bufferId{ submission.vertexBuffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer.isExternalBuffer()) {
                buffer.addBufferUsage(GhaBuffer::UsageMode::VertexBuffer);
            }
            buffer.addReadPass(renderPass);
        }

        {
            RgResourceId const bufferId{ submission.indexBuffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer.isExternalBuffer()) {
                buffer.addBufferUsage(GhaBuffer::UsageMode::IndexBuffer);
            }
            buffer.addReadPass(renderPass);
        }

        for(auto const &ubo : submission.readUniformBuffers) {
            RgResourceId const bufferId{ ubo.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer.isExternalBuffer()) {
                buffer.addBufferUsage(GhaBuffer::UsageMode::UniformBuffer);
            }
            buffer.addReadPass(renderPass);
        }

        for(auto const &sbo : submission.readStorageBuffers) {
            RgResourceId const bufferId{ sbo.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer.isExternalBuffer()) {
                buffer.addBufferUsage(GhaBuffer::UsageMode::StorageBuffer);
            }
            buffer.addReadPass(renderPass);
        }

        for(auto &shaderImage : submission.images) {
            RgImageId const imageId{ shaderImage.imageView.image };
            auto &image{ images.at(imageId) };

            if(!image.isExternalImage()) {
                image.addImageUsage(GhaImage::UsageMode::Sampled);
            }
            image.addReadPass(renderPass);
        }

        pass.addSubmission(std::move(submission));
    }

    void RenderGraph::addComputeSubmission(RgPassId const computePass, RgComputePass::Submission submission) {
        RgComputePass *const pass{ getComputePassFromId(computePass) };
        if(pass == nullptr) {
            CLOVE_LOG(CloveRenderGraph, LogLevel::Error, "Compute pass with ID {0} does not exist.", computePass);
            return;
        }

        for(auto const &ubo : submission.readUniformBuffers) {
            RgResourceId const bufferId{ ubo.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer.isExternalBuffer()) {
                buffer.addBufferUsage(GhaBuffer::UsageMode::UniformBuffer);
            }
            buffer.addReadPass(computePass);
        }

        for(auto const &sbo : submission.readStorageBuffers) {
            RgResourceId const bufferId{ sbo.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer.isExternalBuffer()) {
                buffer.addBufferUsage(GhaBuffer::UsageMode::StorageBuffer);
            }
            buffer.addReadPass(computePass);
        }

        for(auto const &wb : submission.writeBuffers) {
            RgResourceId const bufferId{ wb.buffer };

            auto &buffer{ buffers.at(bufferId) };
            if(!buffer.isExternalBuffer()) {
                buffer.addBufferUsage(GhaBuffer::UsageMode::StorageBuffer);
            }
            buffer.addWritePass(computePass);
        }

        for(auto const &readImage : submission.readImages) {
            RgResourceId const imageId{ readImage.imageView.image };

            auto &image{ images.at(imageId) };
            if(!image.isExternalImage()) {
                image.addImageUsage(GhaImage::UsageMode::Sampled);
            }
            image.addReadPass(computePass);
        }

        for(auto const &writeImage : submission.writeImages) {
            RgResourceId const imageId{ writeImage.imageView.image };

            auto &image{ images.at(imageId) };
            if(!image.isExternalImage()) {
                image.addImageUsage(GhaImage::UsageMode::Storage);
            }
            image.addReadPass(computePass);
        }

        pass->addSubmission(std::move(submission));
    }

    void RenderGraph::execute(ExecutionInfo const &info) {
        CLOVE_ASSERT_MSG(outputResource != INVALID_RESOURCE_ID, "No output resource has been specified");

        //Build the array of passes to execute. Only adding passes which will ultimately effect the output resource.
        std::vector<RgPassId> executionPasses{};
        buildExecutionPasses(executionPasses, outputResource);

        //Reservse order then filter out any duplicates. We reverse order first to make sure we use the earliest pass if any duplicates exist
        std::reverse(executionPasses.begin(), executionPasses.end());
        {
            std::unordered_set<RgPassId> seenPasses{};
            for(auto iter{ executionPasses.begin() }; iter != executionPasses.end();) {
                if(!seenPasses.contains(*iter)) {
                    seenPasses.emplace(*iter);
                    ++iter;
                } else {
                    iter = executionPasses.erase(iter);
                }
            }
        }

        //Build a list of dependecies between the passes. These dependecies signify anything that needs a sempahore.
        std::vector<PassDependency> const passDependencies{ buildDependencies(executionPasses) };

        //Graphics GHA objects.
        std::unordered_map<RgPassId, GhaRenderPass *> allocatedRenderPasses{};
        std::unordered_map<RgPassId, GhaFramebuffer *> allocatedFramebuffers{};
        std::unordered_map<RgPassId, GhaGraphicsPipelineObject *> allocatedGraphicsPipelines{};
        std::unordered_map<RgResourceId, GhaSampler *> allocatedSamplers{};

        //Compute GHA objects.
        std::unordered_map<RgPassId, GhaComputePipelineObject *> allocatedComputePipelines{};

        //Shared GHA objects.
        std::unordered_map<RgPassId, GhaDescriptorSetLayout *> descriptorSetLayouts{};
        std::unordered_map<RgPassId, std::vector<std::unique_ptr<GhaDescriptorSet>>> allocatedDescriptorSets{};

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
            if(buffer.isExternalBuffer()) {
                continue;
            }

            bool isInGraphics{ false };
            bool isInAsyncCompute{ false };

            for(RgPassId passId : buffer.getReadPasses()) {
                if(renderPasses.contains(passId)) {
                    isInGraphics = true;
                }
                if(asyncComputePasses.contains(passId)) {
                    isInAsyncCompute = true;
                }
            }

            for(RgPassId passId : buffer.getWritePasses()) {
                if(renderPasses.contains(passId)) {
                    isInGraphics = true;
                }
                if(asyncComputePasses.contains(passId)) {
                    isInAsyncCompute = true;
                }
            }

            if(isInGraphics && isInAsyncCompute) {
                buffer.setSharingMode(SharingMode::Concurrent);
            }
        }

        for(uint32_t passIndex{ 0 }; passIndex < executionPasses.size(); ++passIndex) {
            RgPassId const passId{ executionPasses[passIndex] };

            //Construct any synchronisation objects the pass will need
            std::vector<std::pair<GhaSemaphore const *, PipelineStage>> waitSemaphores{};
            std::vector<GhaSemaphore const *> signalSemaphores{};
            GhaFence *signalFence{ nullptr };

            for(auto const &dependency : passDependencies) {
                if(passId == dependency.waitPass) {
                    waitSemaphores.emplace_back(dependency.semaphore, dependency.waitStage);
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
                executeGraphicsPass(executionPasses, static_cast<int32_t>(passIndex), *graphicsCommandBufffer, allocatedRenderPasses, allocatedFramebuffers, allocatedGraphicsPipelines, allocatedSamplers, allocatedDescriptorSets);
                graphicsCommandBufffer->endRecording();

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
                executeComputePass(executionPasses, static_cast<int32_t>(passIndex), *computeCommandBufffer, allocatedComputePipelines, allocatedDescriptorSets);
                computeCommandBufffer->endRecording();

                frameCache.submit(ComputeSubmitInfo{
                                      .waitSemaphores   = waitSemaphores,
                                      .commandBuffers   = { computeCommandBufffer },
                                      .signalSemaphores = signalSemaphores,
                                  },
                                  signalFence,
                                  RgSyncType::Sync);

                continue;
            }

            if(asyncComputePasses.contains(passId)) {
                GhaComputeCommandBuffer *computeCommandBufffer{ frameCache.allocateAsyncComputeCommandBuffer() };

                computeCommandBufffer->beginRecording(CommandBufferUsage::OneTimeSubmit);
                executeComputePass(executionPasses, static_cast<int32_t>(passIndex), *computeCommandBufffer, allocatedComputePipelines, allocatedDescriptorSets);
                computeCommandBufffer->endRecording();

                frameCache.submit(ComputeSubmitInfo{
                                      .waitSemaphores   = waitSemaphores,
                                      .commandBuffers   = { computeCommandBufffer },
                                      .signalSemaphores = signalSemaphores,
                                  },
                                  signalFence,
                                  RgSyncType::Async);

                continue;
            }

            if(transferPasses.contains(passId)) {
                RgTransferPass::BufferWrite const &writeOp{ transferPasses.at(passId).getWriteOperation() };
                RgBuffer &buffer{ buffers.at(writeOp.bufferId) };
                buffer.getGhaBuffer(frameCache)->write(writeOp.data.data(), buffer.getBufferOffset() + writeOp.offset, writeOp.size);

                continue;
            }
        }
    }

    void RenderGraph::buildExecutionPasses(std::vector<RgPassId> &outPasses, RgResourceId resourceId) {
        RgResource const *const resource{ getResourceFromId(resourceId) };
        if(resource == nullptr) {
            return;
        }

        std::vector<RgPassId> resourceWritePasses{};
        for(RgPassId passId : resource->getWritePasses()) {
            resourceWritePasses.push_back(passId);
        }

        outPasses.insert(outPasses.end(), resourceWritePasses.begin(), resourceWritePasses.end());

        for(RgPassId passId : resourceWritePasses) {
            RgPass const *const pass{ getPassFromId(passId) };
            if(pass == nullptr) {
                return;
            }

            for(RgResourceId passInputResource : pass->getInputResources()) {
                //Some passes can both read and write to it's depth target so we need to make sure we don't hit any infinite loops
                if(getResourceFromId(passInputResource) != resource) {
                    buildExecutionPasses(outPasses, passInputResource);
                }
            }
        }
    }

    std::vector<RenderGraph::PassDependency> RenderGraph::buildDependencies(std::vector<RgPassId> const &passes) {
        std::vector<RenderGraph::PassDependency> dependencies{};

        auto const getRenderOrComputePass = [this](RgPassId const passId) -> RgPass * {
            if(renderPasses.contains(passId)) {
                return &renderPasses.at(passId);
            } else if(computePasses.contains(passId)) {
                return &computePasses.at(passId);
            } else {
                return nullptr;
            }
        };

        auto const indexOf = [&passes](RgPassId passId) -> size_t {
            for(size_t i{ 0 }; i < passes.size(); ++i) {
                if(passes[i] == passId) {
                    return i;
                }
            }
            return -1;
        };

        //Dependecies are built for the closest pass that has an output of the current passes inputs. All queue operations complete in order so we only need to wait on the last one
        //First pass - Create dependecies for render pass/sync compute inputs
        for(size_t passIndex{ 0 }; passIndex < passes.size(); ++passIndex) {
            RgPassId const passId{ passes[passIndex] };
            if(RgPass const *const dependantPass{ getRenderOrComputePass(passId) }) {
                for(RgResourceId const resourceId : dependantPass->getInputResources()) {
                    RgResource const *const resource{ getResourceFromId(resourceId) };
                    if(resource == nullptr) {
                        continue;
                    }

                    RgPassId dependencyId{ 0 };
                    size_t currentDistance{ -1u };
                    bool hasDependency{ false };

                    for(RgPassId const dependencyPassId : resource->getWritePasses()) {
                        //Only check async compute passes for dependencies as all passes will execute on the same queue.
                        if(asyncComputePasses.contains(dependencyPassId)) {
                            size_t const dependencyIndex{ indexOf(dependencyPassId) };
                            //Dependecy needs to be before this pass.
                            if(dependencyIndex > passIndex) {
                                continue;
                            }

                            //Create a dependency for the closest async compute pass to this pass as the compute pass will
                            //wait on any prior passes anyway.
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
                        //If a pass before this one has a dependency then we don't need to wait because all queue submissions are done in order.
                        for(PassDependency const &dependency : dependencies) {
                            if((renderPasses.contains(dependency.waitPass) || computePasses.contains(dependency.waitPass)) && indexOf(dependency.waitPass) < passIndex) {
                                createDependency = false;
                                break;
                            }
                        }

                        if(createDependency) {
                            PipelineStage waitStage{ PipelineStage::Top };
                            if(renderPasses.contains(passId)) {
                                //Get the correct waitstage for the dependency.
                                //If it's an image we can always assume PipelineStage::PixelShader for now because we only check input resources.
                                //If it's a buffer we check the submission info if it'll be PipelineStage::VertexShader or PipelineStage::PixelShader
                                if(images.contains(resourceId)) {
                                    waitStage = PipelineStage::PixelShader;
                                    //TODO: Might need to use early / late fragment tests if the image is a depth buffer
                                } else if(buffers.contains(resourceId)) {
                                    for(auto const &submission : renderPasses.at(passId).getSubmissions()) {
                                        if(submission.vertexBuffer == resourceId || submission.indexBuffer == resourceId) {
                                            waitStage = PipelineStage::VertexInput;
                                            break;
                                        }

                                        bool found{ false };
                                        for(auto const &ubo : submission.readUniformBuffers) {
                                            if(ubo.slot == resourceId) {
                                                if(ubo.shaderStage == GhaShader::Stage::Vertex) {
                                                    waitStage = PipelineStage::VertexShader;
                                                    found     = true;
                                                    break;
                                                } else if(ubo.shaderStage == GhaShader::Stage::Pixel) {
                                                    waitStage = PipelineStage::PixelShader;
                                                    found     = true;
                                                    break;
                                                }
                                            }
                                        }

                                        if(found) {
                                            break;
                                        }

                                        for(auto const &sbo : submission.readStorageBuffers) {
                                            if(sbo.slot == resourceId) {
                                                if(sbo.shaderStage == GhaShader::Stage::Vertex) {
                                                    waitStage = PipelineStage::VertexShader;
                                                    found     = true;
                                                    break;
                                                } else if(sbo.shaderStage == GhaShader::Stage::Pixel) {
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
                                    CLOVE_LOG(CloveRenderGraph, LogLevel::Warning, "{0}: Could not decide waitStage for resource {1}. Wait stage defaulted to PipelineStage::Top", CLOVE_FUNCTION_NAME_PRETTY, resourceId);
                                }
                            } else if(computePasses.contains(passId)) {
                                waitStage = PipelineStage::ComputeShader;
                            } else {
                                CLOVE_LOG(CloveRenderGraph, LogLevel::Error, "{0}: Pass {0} is not a render passs or a compute pass. Dependecy is likely incorrect", CLOVE_FUNCTION_NAME_PRETTY, passId);
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

        //Second pass - create dependencies for async compute pass inputs.
        for(size_t passIndex{ 0 }; passIndex < passes.size(); ++passIndex) {
            RgPassId const passId{ passes[passIndex] };
            if(computePasses.contains(passId)) {
                for(RgResourceId const resourceId : computePasses.at(passId).getInputResources()) {
                    RgResource const *const resource{ getResourceFromId(resourceId) };
                    if(resource == nullptr) {
                        continue;
                    }

                    RgPassId dependencyId{ 0 };
                    size_t currentDistance{ -1u };
                    bool hasDependency{ false };

                    for(RgPassId const dependencyPassId : resource->getWritePasses()) {
                        if(renderPasses.contains(dependencyPassId) || computePasses.contains(dependencyPassId)) {
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
                            if((renderPasses.contains(dependency.waitPass) || computePasses.contains(dependency.waitPass)) && indexOf(dependency.waitPass) < passIndex) {
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

    GhaImage::Layout RenderGraph::getPreviousLayout(RgImageView const &imageView, std::vector<RgPassId> const &passes, int32_t const currentPassIndex) {
        for(int32_t i{ currentPassIndex - 1 }; i >= 0; --i) {
            if(renderPasses.contains(passes[i])) {
                RgRenderPass const &renderPass{ renderPasses.at(passes[i]) };

                if(isImageAColourAttachment(imageView, renderPass)) {
                    return GhaImage::Layout::ColourAttachmentOptimal;
                } else if(isImageADepthStencilAttachment(imageView, renderPass)) {
                    return GhaImage::Layout::DepthStencilAttachmentOptimal;
                } else if(renderPass.getInputResources().contains(imageView.image)) {
                    return GhaImage::Layout::ShaderReadOnlyOptimal;
                }
            } else if(computePasses.contains(passes[i])) {
                RgComputePass const &computePass{ computePasses.at(passes[i]) };

                if(computePass.getOutputResources().contains(imageView.image)) {
                    return GhaImage::Layout::General;
                } else if(computePass.getInputResources().contains(imageView.image)) {
                    return GhaImage::Layout::ShaderReadOnlyOptimal;
                }
            }
        }

        //If the image is not used again then return undefined. This should be handled at the function call site.
        return GhaImage::Layout::Undefined;
    }

    GhaImage::Layout RenderGraph::getNextLayout(RgImageView const &imageView, std::vector<RgPassId> const &passes, int32_t const currentPassIndex) {
        for(int32_t i{ currentPassIndex + 1 }; i < passes.size(); ++i) {
            if(renderPasses.contains(passes[i])) {
                RgRenderPass const &renderPass{ renderPasses.at(passes[i]) };

                if(isImageAColourAttachment(imageView, renderPass)) {
                    return GhaImage::Layout::ColourAttachmentOptimal;
                } else if(isImageADepthStencilAttachment(imageView, renderPass)) {
                    return GhaImage::Layout::DepthStencilAttachmentOptimal;
                } else if(renderPass.getInputResources().contains(imageView.image)) {
                    return GhaImage::Layout::ShaderReadOnlyOptimal;
                }
            } else if(computePasses.contains(passes[i])) {
                RgComputePass const &computePass{ computePasses.at(passes[i]) };

                if(computePass.getOutputResources().contains(imageView.image)) {
                    return GhaImage::Layout::General;
                } else if(computePass.getInputResources().contains(imageView.image)) {
                    return GhaImage::Layout::ShaderReadOnlyOptimal;
                }
            }
        }

        //Undefined is used as a default if it's not used again as it could be used for a colour or depth pass.
        return imageView.image == outputResource ? GhaImage::Layout::Present : GhaImage::Layout::Undefined;
    }

    RgResource *RenderGraph::getResourceFromId(RgResourceId resourceId) {
        if(images.contains(resourceId)) {
            return &images.at(resourceId);
        } else if(buffers.contains(resourceId)) {
            return &buffers.at(resourceId);
        } else {
            CLOVE_ASSERT_MSG(false, "Could not resourceId pass ID");
            return nullptr;
        }
    }

    RgPass *RenderGraph::getPassFromId(RgPassId passId) {
        if(RgComputePass * computePass{ getComputePassFromId(passId) }) {
            return computePass;
        }

        if(renderPasses.contains(passId)) {
            return &renderPasses.at(passId);
        } else if(transferPasses.contains(passId)) {
            return &transferPasses.at(passId);
        } else {
            return nullptr;
        }
    }

    RgComputePass *RenderGraph::getComputePassFromId(RgPassId passId) {
        if(computePasses.contains(passId)) {
            return &computePasses.at(passId);
        } else if(asyncComputePasses.contains(passId)) {
            return &asyncComputePasses.at(passId);
        } else {
            return nullptr;
        }
    }

    void RenderGraph::generateRenderPassObjects(std::vector<RgPassId> const &passes, std::unordered_map<RgPassId, GhaRenderPass *> &outRenderPasses, std::unordered_map<RgPassId, GhaFramebuffer *> &outFramebuffers, std::unordered_map<RgPassId, GhaGraphicsPipelineObject *> &outGraphicsPipelines, std::unordered_map<RgResourceId, GhaSampler *> &outSamplers, std::unordered_map<RgPassId, GhaDescriptorSetLayout *> &outDescriptorSetLayouts, std::unordered_map<DescriptorType, uint32_t> &totalDescriptorBindingCount, uint32_t &totalDescriptorSets) {
        for(int32_t i{ 0 }; i < passes.size(); ++i) {
            RgPassId const passId{ passes[i] };
            if(!renderPasses.contains(passId)) {
                continue;
            }

            RgRenderPass::Descriptor const &passDescriptor{ renderPasses.at(passId).getDescriptor() };
            std::vector<RgRenderPass::Submission> const &passSubmissions{ renderPasses.at(passId).getSubmissions() };

            //Build and allocate the render pass
            std::vector<AttachmentDescriptor> colourAttachments{};
            for(auto const &renderTarget : passDescriptor.renderTargets) {
                RgImageView const &renderTargetView{ renderTarget.imageView };

                GhaImage::Layout const initialLayout{ getPreviousLayout(renderTargetView, passes, i) };
                GhaImage::Layout const finalLayout{ getNextLayout(renderTargetView, passes, i) };

                colourAttachments.emplace_back(AttachmentDescriptor{
                    .format         = images.at(renderTargetView.image).getFormat(),
                    .loadOperation  = renderTarget.loadOp,
                    .storeOperation = renderTarget.storeOp,
                    .initialLayout  = initialLayout,
                    .usedLayout     = GhaImage::Layout::ColourAttachmentOptimal,
                    .finalLayout    = finalLayout != GhaImage::Layout::Undefined ? finalLayout : GhaImage::Layout::ColourAttachmentOptimal,
                });
            }

            std::optional<AttachmentDescriptor> depthStencilAttachment{};
            if(RgDepthStencilBinding const &depthStencil{ passDescriptor.depthStencil }; depthStencil.imageView.image != INVALID_RESOURCE_ID) {
                RgImageView const &depthStencilView{ depthStencil.imageView };

                GhaImage::Layout const initialLayout{ getPreviousLayout(depthStencilView, passes, i) };
                GhaImage::Layout const finalLayout{ getNextLayout(depthStencilView, passes, i) };

                depthStencilAttachment = AttachmentDescriptor{
                    .format         = images.at(depthStencilView.image).getFormat(),
                    .loadOperation  = depthStencil.loadOp,
                    .storeOperation = depthStencil.storeOp,
                    .initialLayout  = initialLayout,
                    .usedLayout     = GhaImage::Layout::DepthStencilAttachmentOptimal,
                    .finalLayout    = finalLayout != GhaImage::Layout::Undefined ? finalLayout : GhaImage::Layout::DepthStencilAttachmentOptimal,
                };
            }

            outRenderPasses[passId] = globalCache.createRenderPass(GhaRenderPass::Descriptor{
                .colourAttachments = std::move(colourAttachments),
                .depthAttachment   = depthStencilAttachment.value_or(AttachmentDescriptor{}),
            });

            CLOVE_ASSERT_MSG(!passSubmissions.empty(), "Cannot build a pass with 0 submissions!");

            //Build descriptor layouts using the first pass.
            //TODO: Get this infomation from shader reflection
            std::vector<DescriptorSetBindingInfo> descriptorBindings{};
            for(auto const &ubo : passSubmissions[0].readUniformBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = ubo.slot,
                    .type      = DescriptorType::UniformBuffer,
                    .arraySize = 1,
                    .stage     = ubo.shaderStage,//TODO: provided by shader reflection
                });
            }
            for(auto const &sbo : passSubmissions[0].readStorageBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sbo.slot,
                    .type      = DescriptorType::StorageBuffer,
                    .arraySize = 1,
                    .stage     = sbo.shaderStage,//TODO: provided by shader reflection
                });
            }
            for(auto const &image : passSubmissions[0].images) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = image.slot,
                    .type      = DescriptorType::SampledImage,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Pixel,//TODO: provided by shader reflection
                });
            }
            for(auto const &sampler : passSubmissions[0].samplers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sampler.slot,
                    .type      = DescriptorType::Sampler,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Pixel,//TODO: provided shader reflection
                });
            }

            outDescriptorSetLayouts[passId] = globalCache.createDescriptorSetLayout(GhaDescriptorSetLayout::Descriptor{ .bindings = std::move(descriptorBindings) });

            //TEMP: Using dynamic for now
            AreaDescriptor const viewScissorArea{
                .state = ElementState::Dynamic,
            };

            outGraphicsPipelines[passId] = globalCache.createGraphicsPipelineObject(GhaGraphicsPipelineObject::Descriptor{
                .vertexShader       = shaders.at(passDescriptor.vertexShader),
                .pixelShader        = shaders.at(passDescriptor.pixelShader),
                .vertexInput        = passDescriptor.vertexInput,
                .vertexAttributes   = passDescriptor.vertexAttributes,
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
            std::optional<vec2ui> framebufferSize{};//Use the first valid attachment as the frame buffer size
            std::vector<GhaImageView const *> attachments{};
            for(auto const &renderTarget : passDescriptor.renderTargets) {
                RgImageView const &renderTargetView{ renderTarget.imageView };

                attachments.push_back(images.at(renderTargetView.image).getGhaImageView(frameCache, renderTargetView.viewType, renderTargetView.arrayIndex, renderTargetView.arrayCount));

                if(!framebufferSize.has_value()) {
                    framebufferSize = images.at(renderTargetView.image).getDimensions();
                }
            }
            if(passDescriptor.depthStencil.imageView.image != INVALID_RESOURCE_ID) {
                RgImageView const &depthStencilView{ passDescriptor.depthStencil.imageView };

                attachments.push_back(images.at(depthStencilView.image).getGhaImageView(frameCache, depthStencilView.viewType, depthStencilView.arrayIndex, depthStencilView.arrayCount));

                if(!framebufferSize.has_value()) {
                    framebufferSize = images.at(depthStencilView.image).getDimensions();
                }
            }

            outFramebuffers[passId] = frameCache.allocateFramebuffer(GhaFramebuffer::Descriptor{
                .renderPass  = outRenderPasses.at(passId),
                .attachments = std::move(attachments),
                .width       = framebufferSize->x,
                .height      = framebufferSize->y,
            });

            //Count descriptor sets required for the entire pass
            for(auto const &submission : passSubmissions) {
                bool const hasUbo{ !submission.readUniformBuffers.empty() };
                bool const hasSbo{ !submission.readStorageBuffers.empty() };
                bool const hasImage{ !submission.images.empty() };
                bool const hasImageSampler{ !submission.samplers.empty() };

                if(hasUbo) {
                    totalDescriptorBindingCount[DescriptorType::UniformBuffer] += submission.readUniformBuffers.size();
                }
                if(hasSbo) {
                    totalDescriptorBindingCount[DescriptorType::UniformBuffer] += submission.readStorageBuffers.size();
                }
                if(hasImage) {
                    totalDescriptorBindingCount[DescriptorType::SampledImage] += submission.images.size();
                }
                if(hasImageSampler) {
                    totalDescriptorBindingCount[DescriptorType::Sampler] += submission.samplers.size();
                }

                if(hasUbo || hasSbo || hasImage || hasImageSampler) {
                    ++totalDescriptorSets;//Allocating a single set per submission
                }
            }
        }
    }

    void RenderGraph::generateComputePassObjects(std::vector<RgPassId> const &passes, std::unordered_map<RgPassId, GhaComputePipelineObject *> &outComputePipelines, std::unordered_map<RgPassId, GhaDescriptorSetLayout *> &outDescriptorSetLayouts, std::unordered_map<DescriptorType, uint32_t> &totalDescriptorBindingCount, uint32_t &totalDescriptorSets) {
        for(int32_t i{ 0 }; i < passes.size(); ++i) {
            RgPassId const passId{ passes[i] };
            RgComputePass *const computePass{ getComputePassFromId(passId) };
            if(computePass == nullptr) {
                continue;
            }

            RgComputePass::Descriptor const &passDescriptor{ computePass->getDescriptor() };
            std::vector<RgComputePass::Submission> const &passSubmissions{ computePass->getSubmissions() };

            //Build descriptor layouts using the first pass.
            //TODO: Get this infomation from shader reflection
            std::vector<DescriptorSetBindingInfo> descriptorBindings{};
            for(auto const &ubo : passSubmissions[0].readUniformBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = ubo.slot,
                    .type      = DescriptorType::UniformBuffer,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }
            for(auto const &sbo : passSubmissions[0].readStorageBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sbo.slot,
                    .type      = DescriptorType::StorageBuffer,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }
            for(auto const &sbo : passSubmissions[0].writeBuffers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sbo.slot,
                    .type      = DescriptorType::StorageBuffer,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }
            for(auto const &image : passSubmissions[0].readImages) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = image.slot,
                    .type      = DescriptorType::SampledImage,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }
            for(auto const &image : passSubmissions[0].writeImages) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = image.slot,
                    .type      = DescriptorType::StorageImage,
                    .arraySize = 1,
                    .stage     = GhaShader::Stage::Compute,
                });
            }
            for(auto const &sampler : passSubmissions[0].samplers) {
                descriptorBindings.emplace_back(DescriptorSetBindingInfo{
                    .binding   = sampler.slot,
                    .type      = DescriptorType::Sampler,
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
                bool const hasReadImage{ !submission.readImages.empty() };
                bool const hasWriteImage{ !submission.writeImages.empty() };
                bool const hasSampler{ !submission.samplers.empty() };

                if(hasUbo) {
                    totalDescriptorBindingCount[DescriptorType::UniformBuffer] += submission.readUniformBuffers.size();
                }
                if(hasSbo) {
                    totalDescriptorBindingCount[DescriptorType::StorageBuffer] += submission.readStorageBuffers.size();
                }
                if(hasWriteBuffer) {
                    totalDescriptorBindingCount[DescriptorType::StorageBuffer] += submission.writeBuffers.size();
                }
                if(hasReadImage) {
                    totalDescriptorBindingCount[DescriptorType::SampledImage] += submission.readImages.size();
                }
                if(hasWriteImage) {
                    totalDescriptorBindingCount[DescriptorType::StorageImage] += submission.writeImages.size();
                }
                if(hasSampler) {
                    totalDescriptorBindingCount[DescriptorType::Sampler] += submission.samplers.size();
                }

                if(hasUbo || hasSbo || hasWriteBuffer || hasReadImage || hasWriteImage || hasSampler) {
                    ++totalDescriptorSets;//Allocating a single set per submission
                }
            }
        }
    }

    std::unordered_map<RgPassId, std::vector<std::unique_ptr<GhaDescriptorSet>>> RenderGraph::createDescriptorSets(std::unordered_map<DescriptorType, uint32_t> const &totalDescriptorBindingCount, uint32_t const totalDescriptorSets, std::unordered_map<RgPassId, GhaGraphicsPipelineObject *> const &graphicsPipelines, std::unordered_map<RgPassId, GhaComputePipelineObject *> &computePipelines) {
        std::unordered_map<RgPassId, std::vector<std::unique_ptr<GhaDescriptorSet>>> descriptorSets{};

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
            std::vector<GhaDescriptorSetLayout const *> layouts(renderPasses.at(id).getSubmissions().size(), pipeline->getDescriptor().descriptorSetLayouts[0]);//TEMP: Using first index as we know pipelines always have a single descriptor for now
            descriptorSets[id] = descriptorPool->allocateDescriptorSets(layouts);
        }
        for(auto &&[id, pipeline] : computePipelines) {
            if(computePasses.contains(id)) {
                std::vector<GhaDescriptorSetLayout const *> layouts(computePasses.at(id).getSubmissions().size(), pipeline->getDescriptor().descriptorSetLayouts[0]);//TEMP: Using first index as we know pipelines always have a single descriptor for now
                descriptorSets[id] = descriptorPool->allocateDescriptorSets(layouts);
            } else {
                std::vector<GhaDescriptorSetLayout const *> layouts(asyncComputePasses.at(id).getSubmissions().size(), pipeline->getDescriptor().descriptorSetLayouts[0]);//TEMP: Using first index as we know pipelines always have a single descriptor for now
                descriptorSets[id] = descriptorPool->allocateDescriptorSets(layouts);
            }
        }

        return descriptorSets;
    }

    void RenderGraph::executeGraphicsPass(std::vector<RgPassId> const &passes, int32_t const currentPassIndex, GhaGraphicsCommandBuffer &graphicsCommandBufffer, std::unordered_map<RgPassId, GhaRenderPass *> const &allocatedRenderPasses, std::unordered_map<RgPassId, GhaFramebuffer *> const &allocatedFramebuffers, std::unordered_map<RgPassId, GhaGraphicsPipelineObject *> const &allocatedGraphicsPipelines, std::unordered_map<RgResourceId, GhaSampler *> const &allocatedSamplers, std::unordered_map<RgPassId, std::vector<std::unique_ptr<GhaDescriptorSet>>> const &allocatedDescriptorSets) {
        RgPassId const passId{ passes[currentPassIndex] };

        RgRenderPass::Descriptor const &passDescriptor{ renderPasses.at(passId).getDescriptor() };
        std::vector<RgRenderPass::Submission> const &passSubmissions{ renderPasses.at(passId).getSubmissions() };

        //TODO: Batch operations by render pass and start the pass outside of the operation
        RenderArea renderArea{
            .origin = passDescriptor.viewportPosition,
            .size   = passDescriptor.viewportSize,
        };

        std::vector<ClearValue> clearValues{};
        for(auto const &target : passDescriptor.renderTargets) {
            clearValues.push_back(target.clearValue);
        }
        clearValues.emplace_back(passDescriptor.depthStencil.clearValue);

        //Transition any used image's layout if required. We can skip this for colour/depth attachments as the render pass will handle the transition
        std::unordered_set<RgImageId> transitionedImages{};

        for(auto const &submission : passSubmissions) {
            for(auto const &image : submission.images) {
                RgImageView const &imageView{ image.imageView };
                if(transitionedImages.contains(imageView.image)) {
                    continue;
                }

                GhaImage::Layout const currentLayout{ getPreviousLayout(imageView, passes, currentPassIndex) };
                //Renderpasses transition resources for us so if the previous layout is an attachment we can skip it
                if(currentLayout == GhaImage::Layout::ColourAttachmentOptimal || currentLayout == GhaImage::Layout::DepthStencilAttachmentOptimal) {
                    continue;
                }

                GhaImage::Layout const requiredLayout{ GhaImage::Layout::ShaderReadOnlyOptimal };

                if(currentLayout != requiredLayout) {
                    PipelineStage sourcePipelineStage{ PipelineStage::Top };
                    if(currentPassIndex > 0) {
                        int32_t previousPassIndex{ currentPassIndex };
                        do {
                            --previousPassIndex;
                        } while(transferPasses.contains(previousPassIndex));

                        if(renderPasses.contains(previousPassIndex)) {
                            sourcePipelineStage = PipelineStage::PixelShader;
                        } else if(computePasses.contains(previousPassIndex)) {
                            sourcePipelineStage = PipelineStage::ComputeShader;
                        }
                    }

                    ImageMemoryBarrierInfo const memoryBarrier{
                        .currentAccess      = currentLayout == GhaImage::Layout::General ? AccessFlags::ShaderWrite : AccessFlags::ShaderRead,
                        .newAccess          = AccessFlags::ShaderRead,
                        .currentImageLayout = currentLayout,
                        .newImageLayout     = requiredLayout,
                        .baseArrayLayer     = imageView.arrayIndex,
                        .layerCount         = imageView.arrayCount,
                    };
                    graphicsCommandBufffer.imageMemoryBarrier(*images.at(imageView.image).getGhaImage(frameCache), memoryBarrier, sourcePipelineStage, PipelineStage::PixelShader);
                }

                transitionedImages.emplace(imageView.image);
            }
        }

        graphicsCommandBufffer.beginRenderPass(*allocatedRenderPasses.at(passId), *allocatedFramebuffers.at(passId), renderArea, clearValues);

        //TODO: Only do this if viewport is dynamic
        graphicsCommandBufffer.setViewport(passDescriptor.viewportPosition, passDescriptor.viewportSize);
        graphicsCommandBufffer.setScissor({ 0, 0 }, passDescriptor.viewportSize);

        graphicsCommandBufffer.bindPipelineObject(*allocatedGraphicsPipelines.at(passId));

        for(size_t index{ 0 }; auto const &submission : passSubmissions) {
            std::unique_ptr<GhaDescriptorSet> const &descriptorSet{ allocatedDescriptorSets.at(passId)[index] };

            for(auto const &ubo : submission.readUniformBuffers) {
                RgBuffer &buffer{ buffers.at(ubo.buffer) };
                descriptorSet->write(*buffer.getGhaBuffer(frameCache), buffer.getBufferOffset() + ubo.offset, ubo.size, DescriptorType::UniformBuffer, ubo.slot);
            }
            for(auto const &sbo : submission.readStorageBuffers) {
                RgBuffer &buffer{ buffers.at(sbo.buffer) };
                descriptorSet->write(*buffer.getGhaBuffer(frameCache), buffer.getBufferOffset() + sbo.offset, sbo.size, DescriptorType::StorageBuffer, sbo.slot);
            }
            for(auto const &image : submission.images) {
                descriptorSet->write(*images.at(image.imageView.image).getGhaImageView(frameCache, image.imageView.viewType, image.imageView.arrayIndex, image.imageView.arrayCount), GhaImage::Layout::ShaderReadOnlyOptimal, DescriptorType::SampledImage, image.slot);
            }
            for(auto const &sampler : submission.samplers) {
                descriptorSet->write(*samplers.at(sampler.sampler), sampler.slot);
            }

            graphicsCommandBufffer.bindDescriptorSet(*descriptorSet, 0);//TODO: Multiple sets / only set sets for a whole pass (i.e. view)

            RgBuffer &vertexBuffer{ buffers.at(submission.vertexBuffer) };
            RgBuffer &indexBuffer{ buffers.at(submission.indexBuffer) };
            graphicsCommandBufffer.bindVertexBuffer(*vertexBuffer.getGhaBuffer(frameCache), vertexBuffer.getBufferOffset());
            graphicsCommandBufffer.bindIndexBuffer(*indexBuffer.getGhaBuffer(frameCache), indexBuffer.getBufferOffset(), IndexType::Uint16);

            graphicsCommandBufffer.drawIndexed(submission.indexCount);

            ++index;
        }

        graphicsCommandBufffer.endRenderPass();
    }

    void RenderGraph::executeComputePass(std::vector<RgPassId> const &passes, int32_t const currentPassIndex, GhaComputeCommandBuffer &computeCommandBufffer, std::unordered_map<RgPassId, GhaComputePipelineObject *> const &allocatedComputePipelines, std::unordered_map<RgPassId, std::vector<std::unique_ptr<GhaDescriptorSet>>> const &allocatedDescriptorSets) {
        RgPassId const passId{ passes[currentPassIndex] };

        RgComputePass *const computePass{ getComputePassFromId(passId) };
        if(computePass == nullptr) {
            CLOVE_ASSERT(false);
            return;
        }

        RgComputePass::Descriptor const &passDescriptor{ computePass->getDescriptor() };
        std::vector<RgComputePass::Submission> const &passSubmissions{ computePass->getSubmissions() };

        //Transition any used image's layout if required
        std::unordered_set<RgImageId> transitionedImages{};

        auto const addLayoutBarrier = [&](std::vector<RgImageBinding> const &passImages, GhaImage::Layout const requiredLayout) {
            for(auto const &image : passImages) {
                RgImageView const &imageView{ image.imageView };
                if(transitionedImages.contains(imageView.image)) {
                    continue;
                }

                GhaImage::Layout const currentLayout{ getPreviousLayout(imageView, passes, currentPassIndex) };
                //Renderpasses transition resources for us so if the previous layout is an attachment we can skip it
                if(currentLayout == GhaImage::Layout::ColourAttachmentOptimal || currentLayout == GhaImage::Layout::DepthStencilAttachmentOptimal) {
                    continue;
                }

                if(currentLayout != requiredLayout) {
                    PipelineStage sourcePipelineStage{ PipelineStage::Top };
                    if(currentPassIndex > 0) {
                        int32_t previousPassIndex{ currentPassIndex };
                        do {
                            --previousPassIndex;
                        } while(transferPasses.contains(previousPassIndex));

                        if(renderPasses.contains(previousPassIndex)) {
                            sourcePipelineStage = PipelineStage::PixelShader;
                        } else if(computePasses.contains(previousPassIndex)) {
                            sourcePipelineStage = PipelineStage::ComputeShader;
                        }
                    }

                    ImageMemoryBarrierInfo const memoryBarrier{
                        .currentAccess      = currentLayout == GhaImage::Layout::General ? AccessFlags::ShaderWrite : AccessFlags::ShaderRead,
                        .newAccess          = requiredLayout == GhaImage::Layout::General ? AccessFlags::ShaderWrite : AccessFlags::ShaderRead,
                        .currentImageLayout = currentLayout,
                        .newImageLayout     = requiredLayout,
                        .baseArrayLayer     = imageView.arrayIndex,
                        .layerCount         = imageView.arrayCount,
                    };
                    computeCommandBufffer.imageMemoryBarrier(*images.at(imageView.image).getGhaImage(frameCache), memoryBarrier, sourcePipelineStage, PipelineStage::ComputeShader);
                }

                transitionedImages.emplace(imageView.image);
            }
        };

        for(auto const &submission : passSubmissions) {
            addLayoutBarrier(submission.readImages, GhaImage::Layout::ShaderReadOnlyOptimal);
            addLayoutBarrier(submission.writeImages, GhaImage::Layout::General);
        }

        computeCommandBufffer.bindPipelineObject(*allocatedComputePipelines.at(passId));

        for(size_t index{ 0 }; auto const &submission : passSubmissions) {
            std::unique_ptr<GhaDescriptorSet> const &descriptorSet{ allocatedDescriptorSets.at(passId)[index] };

            for(auto const &readUB : submission.readUniformBuffers) {
                RgBuffer &buffer{ buffers.at(readUB.buffer) };
                descriptorSet->write(*buffer.getGhaBuffer(frameCache), buffer.getBufferOffset() + readUB.offset, readUB.size, DescriptorType::UniformBuffer, readUB.slot);
            }
            for(auto const &readSB : submission.readStorageBuffers) {
                RgBuffer &buffer{ buffers.at(readSB.buffer) };
                descriptorSet->write(*buffer.getGhaBuffer(frameCache), buffer.getBufferOffset() + readSB.offset, readSB.size, DescriptorType::StorageBuffer, readSB.slot);
            }
            for(auto const &writeSB : submission.writeBuffers) {
                RgBuffer &buffer{ buffers.at(writeSB.buffer) };
                descriptorSet->write(*buffer.getGhaBuffer(frameCache), buffer.getBufferOffset() + writeSB.offset, writeSB.size, DescriptorType::StorageBuffer, writeSB.slot);
            }
            for(auto const &image : submission.readImages) {
                descriptorSet->write(*images.at(image.imageView.image).getGhaImageView(frameCache, image.imageView.viewType, image.imageView.arrayIndex, image.imageView.arrayCount), GhaImage::Layout::ShaderReadOnlyOptimal, DescriptorType::SampledImage, image.slot);
            }
            for(auto const &image : submission.writeImages) {
                descriptorSet->write(*images.at(image.imageView.image).getGhaImageView(frameCache, image.imageView.viewType, image.imageView.arrayIndex, image.imageView.arrayCount), GhaImage::Layout::General, DescriptorType::StorageImage, image.slot);
            }
            for(auto const &sampler : submission.samplers) {
                descriptorSet->write(*samplers.at(sampler.sampler), sampler.slot);
            }

            computeCommandBufffer.bindDescriptorSet(*descriptorSet, 0);

            computeCommandBufffer.disptach(submission.disptachSize);

            ++index;
        }
    }
}
