#include "Clove/Graphics/Vulkan/VulkanImageView.hpp"

#include "Clove/Graphics/Vulkan/VulkanLog.hpp"

namespace clove {
    VulkanImageView::VulkanImageView(GhaImage::Format viewedFormat, vec2ui viewedDimensions, VkDevice device, VkImageView imageView)
        : viewedFormat{ viewedFormat }
        , viewedDimensions{ viewedDimensions }
        , device{ device }
        , imageView{ imageView } {
    }

    VulkanImageView::VulkanImageView(VulkanImageView &&other) noexcept = default;

    VulkanImageView &VulkanImageView::operator=(VulkanImageView &&other) noexcept = default;

    VulkanImageView::~VulkanImageView() {
        vkDestroyImageView(device, imageView, nullptr);
    }

    GhaImage::Format VulkanImageView::getImageFormat() const {
        return viewedFormat;
    }

    vec2ui const &VulkanImageView::getImageDimensions() const {
        return viewedDimensions;
    }

    VkImageViewType VulkanImageView::convertType(GhaImageView::Type garlicImageType, uint32_t const layerCount) {
        switch(garlicImageType) {
            case GhaImageView::Type::_2D:
                return VK_IMAGE_VIEW_TYPE_2D;
            case GhaImageView::Type::_2DArray:
                return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case GhaImageView::Type::_3D:
                return VK_IMAGE_VIEW_TYPE_3D;
            case GhaImageView::Type::Cube:
                return VK_IMAGE_VIEW_TYPE_CUBE;
            case GhaImageView::Type::CubeArray:
                return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            default:
                CLOVE_ASSERT_MSG(false, "{0}: Unhandled image type");
                return VK_IMAGE_VIEW_TYPE_2D;
        }
    }
}