#include "Clove/Graphics/Vulkan/VulkanRenderPass.hpp"

#include "Clove/Graphics/Vulkan/VulkanGraphicsPipelineObject.hpp"
#include "Clove/Graphics/Vulkan/VulkanImage.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    VulkanRenderPass::VulkanRenderPass(DevicePointer device, VkRenderPass renderPass)
        : device{ std::move(device) }
        , renderPass{ renderPass } {
    }

    VulkanRenderPass::VulkanRenderPass(VulkanRenderPass &&other) noexcept = default;

    VulkanRenderPass &VulkanRenderPass::operator=(VulkanRenderPass &&other) noexcept = default;

    VulkanRenderPass::~VulkanRenderPass() {
        vkDestroyRenderPass(device.get(), renderPass, nullptr);
    }

    VkRenderPass VulkanRenderPass::getRenderPass() const {
        return renderPass;
    }
}