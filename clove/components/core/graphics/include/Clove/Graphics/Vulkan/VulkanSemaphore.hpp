#pragma once

#include "Clove/Graphics/GhaSemaphore.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanSemaphore : public GhaSemaphore {
        //VARIABLES
    private:
        DevicePointer device;

        VkSemaphore semaphore{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanSemaphore() = delete;
        VulkanSemaphore(DevicePointer device, VkSemaphore semaphore);

        VulkanSemaphore(VulkanSemaphore const &other) = delete;
        VulkanSemaphore(VulkanSemaphore &&other) noexcept;

        VulkanSemaphore &operator=(VulkanSemaphore const &other) = delete;
        VulkanSemaphore &operator=(VulkanSemaphore &&other) noexcept;

        ~VulkanSemaphore();

        VkSemaphore getSemaphore() const;
    };
}