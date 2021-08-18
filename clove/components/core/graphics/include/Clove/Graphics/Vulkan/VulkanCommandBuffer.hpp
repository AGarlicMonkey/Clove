#pragma once

#include "Clove/Graphics/CommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanMemoryBarrier.hpp"
#include "Clove/Graphics/Vulkan/VulkanPipelineObject.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class GhaBuffer;
}

namespace clove {
    VkCommandBufferUsageFlags getCommandBufferUsageFlags(CommandBufferUsage garlicUsage);

    void createBufferMemoryBarrier(VkCommandBuffer vkCommandBuffer, QueueFamilyIndices const &queueFamilyIndices, GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage);
    void createImageMemoryBarrier(VkCommandBuffer vkCommandBuffer, QueueFamilyIndices const &queueFamilyIndices, GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage);
}