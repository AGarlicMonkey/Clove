#include "Clove/Graphics/Vulkan/VulkanGraphicsPipelineObject.hpp"

namespace clove {
    VulkanGraphicsPipelineObject::VulkanGraphicsPipelineObject(DevicePointer device, VkPipeline pipeline, VkPipelineLayout pipelineLayout)
        : device{ std::move(device) }
        , pipeline{ pipeline }
        , pipelineLayout{ pipelineLayout } {
    }

    VulkanGraphicsPipelineObject::VulkanGraphicsPipelineObject(VulkanGraphicsPipelineObject &&other) noexcept = default;

    VulkanGraphicsPipelineObject &VulkanGraphicsPipelineObject::operator=(VulkanGraphicsPipelineObject &&other) noexcept = default;

    VulkanGraphicsPipelineObject::~VulkanGraphicsPipelineObject() {
        vkDestroyPipeline(device.get(), pipeline, nullptr);
        vkDestroyPipelineLayout(device.get(), pipelineLayout, nullptr);
    }

    VkPipeline VulkanGraphicsPipelineObject::getPipeline() const {
        return pipeline;
    }

    VkPipelineLayout VulkanGraphicsPipelineObject::getLayout() const {
        return pipelineLayout;
    }
}