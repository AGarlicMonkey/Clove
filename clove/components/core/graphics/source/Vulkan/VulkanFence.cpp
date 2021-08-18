#include "Clove/Graphics/Vulkan/VulkanFence.hpp"

#include <utility>

namespace clove {
    VulkanFence::VulkanFence(DevicePointer device, VkFence fence)
        : device{ std::move(device) }
        , fence{ fence } {
    }

    VulkanFence::VulkanFence(VulkanFence &&other) noexcept = default;

    VulkanFence &VulkanFence::operator=(VulkanFence &&other) noexcept = default;

    VulkanFence::~VulkanFence() {
        vkDestroyFence(device.get(), fence, nullptr);
    }

    void VulkanFence::wait() {
        vkWaitForFences(device.get(), 1, &fence, VK_TRUE, UINT64_MAX);
    }

    void VulkanFence::reset() {
        vkResetFences(device.get(), 1, &fence);
    }

    VkFence VulkanFence::getFence() const {
        return fence;
    }
}