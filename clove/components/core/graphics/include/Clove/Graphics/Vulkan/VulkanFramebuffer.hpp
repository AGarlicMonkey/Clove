#pragma once

#include "Clove/Graphics/GhaFramebuffer.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanFramebuffer : public GhaFramebuffer {
        //VARIABLES
    private:
        DevicePointer device;

        VkFramebuffer framebuffer{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanFramebuffer() = delete;
        VulkanFramebuffer(DevicePointer device, VkFramebuffer framebuffer);

        VulkanFramebuffer(VulkanFramebuffer const &other) = delete;
        VulkanFramebuffer(VulkanFramebuffer &&other) noexcept;

        VulkanFramebuffer &operator=(VulkanFramebuffer const &other) = delete;
        VulkanFramebuffer &operator=(VulkanFramebuffer &&other) noexcept;

        ~VulkanFramebuffer();

        VkFramebuffer getFrameBuffer() const;
    };
}