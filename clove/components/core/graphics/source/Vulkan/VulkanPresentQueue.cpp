//There seems to be a bug with optional in msvc that stops it compiling. Having this header included here (above VulkanPresentQueue) is a current work around.
#include "Clove/Graphics/Vulkan/VulkanSwapchain.hpp"
//
#include "Clove/Graphics/Vulkan/VulkanPresentQueue.hpp"
//

#include "Clove/Graphics/Vulkan/VulkanFence.hpp"
#include "Clove/Graphics/Vulkan/VulkanResult.hpp"
#include "Clove/Graphics/Vulkan/VulkanSemaphore.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    VulkanPresentQueue::VulkanPresentQueue(DevicePointer device, VkQueue queue)
        : device{ std::move(device) }
        , queue{ queue } {
    }

    VulkanPresentQueue::VulkanPresentQueue(VulkanPresentQueue &&other) noexcept = default;

    VulkanPresentQueue &VulkanPresentQueue::operator=(VulkanPresentQueue &&other) noexcept = default;

    VulkanPresentQueue::~VulkanPresentQueue() = default;

    Result VulkanPresentQueue::present(PresentInfo const &presentInfo) {
        //Wait semaphores
        const size_t waitSemaphoreCount = std::size(presentInfo.waitSemaphores);
        std::vector<VkSemaphore> waitSemaphores(waitSemaphoreCount);
        for(size_t i = 0; i < waitSemaphoreCount; ++i) {
            waitSemaphores[i] = polyCast<VulkanSemaphore>(presentInfo.waitSemaphores[i].get())->getSemaphore();
        }

        VkSwapchainKHR const swapchain{ polyCast<VulkanSwapchain>(presentInfo.swapChain.get())->getSwapchain() };
        uint32_t const imageIndex{ presentInfo.imageIndex };

        VkPresentInfoKHR vkpresentInfo{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphoreCount),
            .pWaitSemaphores    = std::data(waitSemaphores),
            .swapchainCount     = 1,
            .pSwapchains        = &swapchain,
            .pImageIndices      = &imageIndex,
        };

        VkResult const result{ vkQueuePresentKHR(queue, &vkpresentInfo) };

        return convertResult(result);
    }
}