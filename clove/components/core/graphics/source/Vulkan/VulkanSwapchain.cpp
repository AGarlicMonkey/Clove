#include "Clove/Graphics/Vulkan/VulkanSwapchain.hpp"

#include "Clove/Graphics/Vulkan/VulkanImage.hpp"
#include "Clove/Graphics/Vulkan/VulkanResult.hpp"
#include "Clove/Graphics/Vulkan/VulkanSemaphore.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    VulkanSwapchain::VulkanSwapchain(DevicePointer device, VkSwapchainKHR swapchain, VkQueue presentQueue, VkFormat swapChainImageFormat, VkExtent2D swapChainExtent, std::vector<std::unique_ptr<VulkanImage>> images)
        : device{ std::move(device) }
        , swapchain{ swapchain }
        , presentQueue{ presentQueue }
        , swapChainImageFormat{ swapChainImageFormat }
        , swapChainExtent{ swapChainExtent }
        , images{ std::move(images) } {
    }

    VulkanSwapchain::VulkanSwapchain(VulkanSwapchain &&other) noexcept = default;

    VulkanSwapchain &VulkanSwapchain::operator=(VulkanSwapchain &&other) noexcept = default;

    VulkanSwapchain::~VulkanSwapchain() {
        vkDestroySwapchainKHR(device.get(), swapchain, nullptr);
    }

    std::pair<GhaImage *, Result> VulkanSwapchain::aquireNextImage(GhaSemaphore const *availableSemaphore) {
        VkSemaphore vkSemaphore{ availableSemaphore != nullptr ? polyCast<VulkanSemaphore const>(availableSemaphore)->getSemaphore() : VK_NULL_HANDLE };
        VkResult const result{ vkAcquireNextImageKHR(device.get(), swapchain, UINT64_MAX, vkSemaphore, VK_NULL_HANDLE, &activeImage) };

        return { result >= VK_SUCCESS ? images[activeImage].get() : nullptr, convertResult(result) };
    }

    uint32_t VulkanSwapchain::getCurrentImageIndex() const {
        return activeImage;
    }

    Result VulkanSwapchain::present(std::vector<GhaSemaphore const *> waitSemaphores) {
        size_t const waitSemaphoreCount{ waitSemaphores.size() };
        std::vector<VkSemaphore> vkSemaphores(waitSemaphoreCount);
        for(size_t i = 0; i < waitSemaphoreCount; ++i) {
            vkSemaphores[i] = polyCast<VulkanSemaphore const>(waitSemaphores[i])->getSemaphore();
        }

        VkPresentInfoKHR const vkpresentInfo{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphoreCount),
            .pWaitSemaphores    = vkSemaphores.data(),
            .swapchainCount     = 1,
            .pSwapchains        = &swapchain,
            .pImageIndices      = &activeImage,
        };

        VkResult const result{ vkQueuePresentKHR(presentQueue, &vkpresentInfo) };

        return convertResult(result);
    }

    GhaImage::Format VulkanSwapchain::getImageFormat() const {
        return VulkanImage::convertFormat(swapChainImageFormat);
    }

    vec2ui VulkanSwapchain::getSize() const {
        return { swapChainExtent.width, swapChainExtent.height };
    }
}