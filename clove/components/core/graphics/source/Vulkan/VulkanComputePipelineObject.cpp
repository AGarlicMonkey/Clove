#include "Clove/Graphics/Vulkan/VulkanComputePipelineObject.hpp"

namespace clove {
    VulkanComputePipelineObject::VulkanComputePipelineObject(DevicePointer device, VkPipeline pipeline, VkPipelineLayout pipelineLayout)
        : device{ std::move(device) }
        , pipeline{ pipeline }
        , pipelineLayout{ pipelineLayout } {
    }

    VulkanComputePipelineObject::VulkanComputePipelineObject(VulkanComputePipelineObject &&other) noexcept = default;

    VulkanComputePipelineObject &VulkanComputePipelineObject::operator=(VulkanComputePipelineObject &&other) noexcept = default;

    VulkanComputePipelineObject::~VulkanComputePipelineObject() {
        vkDestroyPipeline(device.get(), pipeline, nullptr);
        vkDestroyPipelineLayout(device.get(), pipelineLayout, nullptr);
    }

    VkPipeline VulkanComputePipelineObject::getPipeline() const {
        return pipeline;
    }

    VkPipelineLayout VulkanComputePipelineObject::getLayout() const {
        return pipelineLayout;
    }
}