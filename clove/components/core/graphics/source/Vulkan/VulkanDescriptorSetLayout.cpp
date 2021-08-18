#include "Clove/Graphics/Vulkan/VulkanDescriptorSetLayout.hpp"

namespace clove {
    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(DevicePointer device, VkDescriptorSetLayout layout, Descriptor descriptor)
        : device{ std::move(device) }
        , layout{ layout }
        , descriptor{ std::move(descriptor) } {
    }

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayout &&other) noexcept = default;

    VulkanDescriptorSetLayout &VulkanDescriptorSetLayout::operator=(VulkanDescriptorSetLayout &&other) noexcept = default;

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(device.get(), layout, nullptr);
    }
}