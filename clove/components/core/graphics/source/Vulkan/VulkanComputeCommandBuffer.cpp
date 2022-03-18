#include "Clove/Graphics/Vulkan/VulkanComputeCommandBuffer.hpp"

#include "Clove/Graphics/Vulkan/VulkanBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanCommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanComputePipelineObject.hpp"
#include "Clove/Graphics/Vulkan/VulkanDescriptorSet.hpp"
#include "Clove/Graphics/Vulkan/VulkanImage.hpp"
#include "Clove/Graphics/Vulkan/VulkanLog.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    VulkanComputeCommandBuffer::VulkanComputeCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyIndices queueFamilyIndices)
        : commandBuffer{ commandBuffer }
        , queueFamilyIndices{ queueFamilyIndices } {
    }

    VulkanComputeCommandBuffer::VulkanComputeCommandBuffer(VulkanComputeCommandBuffer &&other) noexcept = default;

    VulkanComputeCommandBuffer &VulkanComputeCommandBuffer::operator=(VulkanComputeCommandBuffer &&other) noexcept = default;

    VulkanComputeCommandBuffer::~VulkanComputeCommandBuffer() = default;

    void VulkanComputeCommandBuffer::beginRecording() {
        VkCommandBufferBeginInfo beginInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Failed to begin recording command buffer");
        }
    }

    void VulkanComputeCommandBuffer::endRecording() {
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Failed to end recording command buffer");
        }
    }

    void VulkanComputeCommandBuffer::bindPipelineObject(GhaComputePipelineObject &pipelineObject) {
        auto const *pipeline{ polyCast<VulkanComputePipelineObject>(&pipelineObject) };

        currentLayout = pipeline->getLayout();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipeline());
    }

    void VulkanComputeCommandBuffer::bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) {
        VkDescriptorSet const vkSet{ polyCast<VulkanDescriptorSet>(&descriptorSet)->getDescriptorSet() };

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, currentLayout, setNum, 1, &vkSet, 0, nullptr);
    }

    void VulkanComputeCommandBuffer::pushConstant(size_t const offset, size_t const size, void const *data) {
        vkCmdPushConstants(commandBuffer, currentLayout, VK_SHADER_STAGE_COMPUTE_BIT, offset, size, data);
    }

    void VulkanComputeCommandBuffer::disptach(vec3ui groupCount) {
        vkCmdDispatch(commandBuffer, groupCount.x, groupCount.y, groupCount.z);
    }

    void VulkanComputeCommandBuffer::bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
        createBufferMemoryBarrier(commandBuffer, queueFamilyIndices, buffer, barrierInfo, sourceStage, destinationStage);
    }

    void VulkanComputeCommandBuffer::imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
        createImageMemoryBarrier(commandBuffer, queueFamilyIndices, image, barrierInfo, sourceStage, destinationStage);
    }
}