#pragma once

#include "Clove/Graphics/GhaGraphicsCommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanGraphicsCommandBuffer : public GhaGraphicsCommandBuffer {
        //VARIABLES
    private:
        VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };

        VkPipelineLayout currentLayout{};

        QueueFamilyIndices queueFamilyIndices;

        //FUNCTIONS
    public:
        VulkanGraphicsCommandBuffer() = delete;
        VulkanGraphicsCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyIndices queueFamilyIndices);

        VulkanGraphicsCommandBuffer(VulkanGraphicsCommandBuffer const &other) = delete;
        VulkanGraphicsCommandBuffer(VulkanGraphicsCommandBuffer &&other) noexcept;

        VulkanGraphicsCommandBuffer &operator=(VulkanGraphicsCommandBuffer const &other) = delete;
        VulkanGraphicsCommandBuffer &operator=(VulkanGraphicsCommandBuffer &&other) noexcept;

        ~VulkanGraphicsCommandBuffer();

        void beginRecording(CommandBufferUsage usageFlag) override;
        void endRecording() override;

        void beginRenderPass(GhaRenderPass &renderPass, GhaFramebuffer &frameBuffer, RenderArea const &renderArea, std::span<ClearValue> clearValues) override;
        void endRenderPass() override;

        void setViewport(vec2i position, vec2ui size) override;
        void setScissor(vec2i position, vec2ui size) override;

        void bindPipelineObject(GhaGraphicsPipelineObject &pipelineObject) override;
        void bindVertexBuffer(GhaBuffer &vertexBuffer, size_t const offset) override;
        void bindIndexBuffer(GhaBuffer &indexBuffer, size_t const offset, IndexType indexType) override;
        void bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) override;

        void pushConstant(GhaShader::Stage const stage, size_t const offset, size_t const size, void const *data) override;

        void drawIndexed(size_t const indexCount) override;

        void bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;
        void imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;

        VkCommandBuffer getCommandBuffer() const;
    };
}