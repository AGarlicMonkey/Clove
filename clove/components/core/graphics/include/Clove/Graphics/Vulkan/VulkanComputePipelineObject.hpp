#pragma once

#include "Clove/Graphics/GhaComputePipelineObject.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace garlic::clove {
    class VulkanComputePipelineObject : public GhaComputePipelineObject {
        //VARIABLES
    private:
        Descriptor descriptor{};

        DevicePointer device{};

        VkPipeline pipeline{ VK_NULL_HANDLE };
        VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanComputePipelineObject() = delete;
        VulkanComputePipelineObject(Descriptor descriptor, DevicePointer device, VkPipeline pipeline, VkPipelineLayout pipelineLayout);

        VulkanComputePipelineObject(VulkanComputePipelineObject const &other) = delete;
        VulkanComputePipelineObject(VulkanComputePipelineObject &&other) noexcept;

        VulkanComputePipelineObject &operator=(VulkanComputePipelineObject const &other) = delete;
        VulkanComputePipelineObject &operator=(VulkanComputePipelineObject &&other) noexcept;

        ~VulkanComputePipelineObject();

        Descriptor const &getDescriptor() const override;

        VkPipeline getPipeline() const;
        VkPipelineLayout getLayout() const;
    };
}