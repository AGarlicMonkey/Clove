#pragma once

#include "Clove/Graphics/GhaSampler.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanSampler : public GhaSampler {
        //VARIABLES
    private:
        DevicePointer device;

        VkSampler sampler{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanSampler() = delete;
        VulkanSampler(DevicePointer device, VkSampler sampler);

        VulkanSampler(VulkanSampler const& other) = delete;
        VulkanSampler(VulkanSampler&& other) noexcept;

        VulkanSampler& operator=(VulkanSampler const& other) = delete;
        VulkanSampler& operator=(VulkanSampler&& other) noexcept;

        ~VulkanSampler();

        inline VkSampler getSampler() const;
    };
}

#include "VulkanSampler.inl"