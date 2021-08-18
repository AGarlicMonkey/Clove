#pragma once

#include "Clove/Graphics/GhaImageView.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanImageView : public GhaImageView {
        //VARIABLES
    private:
        VkDevice device = VK_NULL_HANDLE;

        VkImageView imageView = VK_NULL_HANDLE;

        //FUNCTIONS
    public:
        VulkanImageView() = delete;
        VulkanImageView(VkDevice device, VkImageView imageView);

        VulkanImageView(VulkanImageView const& other) = delete;
        VulkanImageView(VulkanImageView&& other) noexcept;

        VulkanImageView& operator=(VulkanImageView const& other) = delete;
        VulkanImageView& operator=(VulkanImageView&& other) noexcept;

        ~VulkanImageView();

        inline VkImageView getImageView() const;

        static VkImageView create(VkDevice device, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t const baseLayer, uint32_t const layerCount);
    };
}

#include "VulkanImageView.inl"