#pragma once

#include "Clove/Graphics/GhaSwapchain.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanImage;
}

namespace clove {
    class VulkanSwapchain : public GhaSwapchain {
        //VARIABLES
    private:
        DevicePointer device;
        VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
        VkQueue presentQueue{ VK_NULL_HANDLE };

        uint32_t activeImage{ 0 };

        VkFormat swapChainImageFormat{};
        VkExtent2D swapChainExtent{};

        std::vector<std::unique_ptr<VulkanImage>> images;

        //FUNCTIONS
    public:
        VulkanSwapchain() = delete;
        VulkanSwapchain(DevicePointer device, VkSwapchainKHR swapchain, VkQueue presentQueue, VkFormat swapChainImageFormat, VkExtent2D swapChainExtent, std::vector<std::unique_ptr<VulkanImage>> images);

        VulkanSwapchain(VulkanSwapchain const &other) = delete;
        VulkanSwapchain(VulkanSwapchain &&other) noexcept;

        VulkanSwapchain &operator=(VulkanSwapchain const &other) = delete;
        VulkanSwapchain &operator=(VulkanSwapchain &&other) noexcept;

        ~VulkanSwapchain();

        std::pair<GhaImage *, Result> aquireNextImage(GhaSemaphore const *availableSemaphore) override;

        uint32_t getCurrentImageIndex() const override;

        Result present(std::vector<GhaSemaphore const *> waitSemaphores) override;

        GhaImage::Format getImageFormat() const override;
        vec2ui getSize() const override;

        inline VkSwapchainKHR getSwapchain() const;
    };
}

#include "VulkanSwapchain.inl"