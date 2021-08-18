#pragma once

#include "Clove/Graphics/GhaDescriptorSetLayout.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanDescriptorSetLayout : public GhaDescriptorSetLayout {
        //VARIABLES
    private:
        DevicePointer device;

        Descriptor descriptor;

        VkDescriptorSetLayout layout{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanDescriptorSetLayout() = delete;
        VulkanDescriptorSetLayout(DevicePointer device, VkDescriptorSetLayout layout, Descriptor descriptor);

        VulkanDescriptorSetLayout(VulkanDescriptorSetLayout const &other) = delete;
        VulkanDescriptorSetLayout(VulkanDescriptorSetLayout &&other) noexcept;

        VulkanDescriptorSetLayout &operator=(VulkanDescriptorSetLayout const &other) = delete;
        VulkanDescriptorSetLayout &operator =(VulkanDescriptorSetLayout &&other) noexcept;

        ~VulkanDescriptorSetLayout();

        inline Descriptor const &getDescriptor() const override;

        inline VkDescriptorSetLayout getLayout() const;
    };
}

#include "VulkanDescriptorSetLayout.inl"