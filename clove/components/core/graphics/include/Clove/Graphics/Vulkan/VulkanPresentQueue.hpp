#pragma once

#include "Clove/Graphics/GhaPresentQueue.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanPresentQueue : public GhaPresentQueue {
        //VARIABLES
    private:
        DevicePointer device;
        VkQueue queue{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanPresentQueue() = delete;
        VulkanPresentQueue(DevicePointer device, VkQueue queue);

        VulkanPresentQueue(const VulkanPresentQueue& other) = delete;
        VulkanPresentQueue(VulkanPresentQueue&& other) noexcept;

        VulkanPresentQueue& operator=(const VulkanPresentQueue& other) = delete;
        VulkanPresentQueue& operator=(VulkanPresentQueue&& other) noexcept;

        ~VulkanPresentQueue();

        Result present(const PresentInfo& presentInfo);
    };
}