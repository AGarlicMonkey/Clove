#pragma once

#include "Clove/Graphics/GhaTransferCommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanTransferCommandBuffer : public GhaTransferCommandBuffer {
        //VARIABLES
    private:
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        QueueFamilyIndices queueFamilyIndices;

        //FUNCTIONS
    public:
        VulkanTransferCommandBuffer() = delete;
        VulkanTransferCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyIndices queueFamilyIndices);

        VulkanTransferCommandBuffer(VulkanTransferCommandBuffer const &other) = delete;
        VulkanTransferCommandBuffer(VulkanTransferCommandBuffer &&other) noexcept;

        VulkanTransferCommandBuffer &operator=(VulkanTransferCommandBuffer const &other) = delete;
        VulkanTransferCommandBuffer &operator=(VulkanTransferCommandBuffer &&other) noexcept;

        ~VulkanTransferCommandBuffer();

        void beginRecording(CommandBufferUsage usageFlag) override;
        void endRecording() override;

        void copyBufferToBuffer(GhaBuffer &source, size_t const sourceOffset, GhaBuffer &destination, size_t const destinationOffset, size_t const sizeBytes) override;
        void copyBufferToImage(GhaBuffer &source, size_t const sourceOffset, GhaImage &destination, vec3i const &destinationOffset, vec3ui const &destinationExtent) override;

        void copyImageToBuffer(GhaImage &source, vec3i const &sourceOffset, vec3ui const &sourceExtent, GhaBuffer &destination, size_t const destinationOffset) override;

        void bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;
        void imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;

        VkCommandBuffer getCommandBuffer() const;
    };
}