#pragma once

#include "Clove/Graphics/GhaComputeCommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanComputeCommandBuffer : public GhaComputeCommandBuffer {
        //VARIABLES
    private:
        VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };

        VkPipelineLayout currentLayout{};

        QueueFamilyIndices queueFamilyIndices;

        //FUNCTIONS
    public:
        VulkanComputeCommandBuffer() = delete;
        VulkanComputeCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyIndices queueFamilyIndices);

        VulkanComputeCommandBuffer(VulkanComputeCommandBuffer const &other) = delete;
        VulkanComputeCommandBuffer(VulkanComputeCommandBuffer &&other) noexcept;

        VulkanComputeCommandBuffer &operator=(VulkanComputeCommandBuffer const &other) = delete;
        VulkanComputeCommandBuffer &operator=(VulkanComputeCommandBuffer &&other) noexcept;

        ~VulkanComputeCommandBuffer();

        void beginRecording(CommandBufferUsage usageFlag) override;
        void endRecording() override;

        void bindPipelineObject(GhaComputePipelineObject &pipelineObject) override;

        void bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) override;
        void pushConstant(size_t const offset, size_t const size, void const *data) override;

        void disptach(vec3ui groupCount) override;

        /**
         * @brief Creates a memory barrier for a buffer. Allowing for how it's accessed to be changed and/or to transfer queue ownership.
         * @param buffer The buffer to create the barrier for.
         * @param barrierInfo The information about the barrier.
         * @param sourceStage The pipeline stage that gets executed before the barrier.
         * @param destinationStage The pipeline stage executed after the barrier that waits for the results of the sourceStage.
         */
        void bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;
        /**
         * @brief Creates a memory barrier for an image. Allowing for how it's accessed, it's layout and queue ownership to change.
         * @param image The image to create the barrier for.
         * @param barrierInfo The information about the barrier.
         * @param sourceStage The pipeline stage that gets executed before the barrier.
         * @param destinationStage The pipeline stage executed after the barrier that waits for the results of the sourceStage.
         */
        void imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;

        VkCommandBuffer getCommandBuffer() const;
    };
}