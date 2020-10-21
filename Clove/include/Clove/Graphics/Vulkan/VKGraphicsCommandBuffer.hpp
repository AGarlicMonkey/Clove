#pragma once

#include "Clove/Graphics/GraphicsCommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clv::gfx::vk {
    class VKGraphicsCommandBuffer : public GraphicsCommandBuffer {
        //VARIABLES
    private:
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        QueueFamilyIndices queueFamilyIndices;

        //FUNCTIONS
    public:
        VKGraphicsCommandBuffer() = delete;
        VKGraphicsCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyIndices queueFamilyIndices);

        VKGraphicsCommandBuffer(VKGraphicsCommandBuffer const& other) = delete;
        VKGraphicsCommandBuffer(VKGraphicsCommandBuffer&& other) noexcept;

        VKGraphicsCommandBuffer& operator=(VKGraphicsCommandBuffer const& other) = delete;
        VKGraphicsCommandBuffer& operator                                        =(VKGraphicsCommandBuffer&& other) noexcept;

        ~VKGraphicsCommandBuffer();

        void beginRecording(CommandBufferUsage usageFlag) override;
        void endRecording() override;

        void beginRenderPass(RenderPass& renderPass, Framebuffer& frameBuffer, RenderArea const& renderArea, std::span<ClearValue> clearValues) override;
        void endRenderPass() override;

        void bindPipelineObject(PipelineObject& pipelineObject) override;
        void bindVertexBuffer(GraphicsBuffer& vertexBuffer, uint32_t const binding) override;
        void bindIndexBuffer(GraphicsBuffer& indexBuffer, IndexType indexType) override;
        void bindDescriptorSet(DescriptorSet& descriptorSet, PipelineObject const& pipeline, uint32_t const setNum) override;

        void pushConstant(PipelineObject& pipelineObject, Shader::Stage const stage, size_t const offset, size_t const size, void const* data) override;

        void drawIndexed(size_t const indexCount) override;

        void bufferMemoryBarrier(GraphicsBuffer& buffer, BufferMemoryBarrierInfo const& barrierInfo, PipelineObject::Stage sourceStage, PipelineObject::Stage destinationStage) override;
        void imageMemoryBarrier(GraphicsImage& image, ImageMemoryBarrierInfo const& barrierInfo, PipelineObject::Stage sourceStage, PipelineObject::Stage destinationStage) override;

        VkCommandBuffer getCommandBuffer() const;
    };
}