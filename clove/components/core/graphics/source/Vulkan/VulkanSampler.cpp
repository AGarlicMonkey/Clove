#include "Clove/Graphics/Vulkan/VulkanSampler.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    VulkanSampler::VulkanSampler(DevicePointer device, VkSampler sampler)
        : device{ std::move(device) }
        , sampler{ sampler } {
    }

    VulkanSampler::VulkanSampler(VulkanSampler &&other) noexcept = default;

    VulkanSampler &VulkanSampler::operator=(VulkanSampler &&other) noexcept = default;

    VulkanSampler::~VulkanSampler() {
        vkDestroySampler(device.get(), sampler, nullptr);
    }
}