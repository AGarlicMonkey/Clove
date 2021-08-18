#include "Clove/Graphics/Vulkan/VulkanSemaphore.hpp"

#include <utility>

namespace clove {
    VulkanSemaphore::VulkanSemaphore(DevicePointer device, VkSemaphore semaphore)
        : device{ std::move(device) }
        , semaphore{ semaphore } {
    }

    VulkanSemaphore::VulkanSemaphore(VulkanSemaphore &&other) noexcept = default;

    VulkanSemaphore &VulkanSemaphore::operator=(VulkanSemaphore &&other) noexcept = default;

    VulkanSemaphore::~VulkanSemaphore() {
        vkDestroySemaphore(device.get(), semaphore, nullptr);
    }

    VkSemaphore VulkanSemaphore::getSemaphore() const {
        return semaphore;
    }
}