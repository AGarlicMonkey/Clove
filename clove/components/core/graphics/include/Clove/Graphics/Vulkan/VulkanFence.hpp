#pragma once

#include "Clove/Graphics/GhaFence.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanFence : public GhaFence {
        //VARIABLES
    private:
        DevicePointer device;

        VkFence fence{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanFence() = delete;
        VulkanFence(DevicePointer device, VkFence fence);

        VulkanFence(VulkanFence const &other) = delete;
        VulkanFence(VulkanFence &&other) noexcept;

        VulkanFence &operator=(VulkanFence const &other) = delete;
        VulkanFence &operator=(VulkanFence &&other) noexcept;

        ~VulkanFence();

        void wait() override;
        void reset() override;

        VkFence getFence() const;
    };
}