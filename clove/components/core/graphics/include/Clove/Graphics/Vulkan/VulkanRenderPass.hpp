#pragma once

#include "Clove/Graphics/GhaRenderPass.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanRenderPass : public GhaRenderPass {
        //VARIABLES
    private:
        DevicePointer device;

        VkRenderPass renderPass{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanRenderPass() = delete;
        VulkanRenderPass(DevicePointer device, VkRenderPass renderPass);

        VulkanRenderPass(VulkanRenderPass const &other) = delete;
        VulkanRenderPass(VulkanRenderPass &&other) noexcept;

        VulkanRenderPass &operator=(VulkanRenderPass const &other) = delete;
        VulkanRenderPass &operator=(VulkanRenderPass &&other) noexcept;

        ~VulkanRenderPass();

        VkRenderPass getRenderPass() const;
    };
}