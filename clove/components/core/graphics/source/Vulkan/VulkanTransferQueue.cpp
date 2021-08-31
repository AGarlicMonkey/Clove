#include "Clove/Graphics/Vulkan/VulkanTransferQueue.hpp"

#include "Clove/Graphics/Helpers.hpp"
#include "Clove/Graphics/Vulkan/VulkanFence.hpp"
#include "Clove/Graphics/Vulkan/VulkanGraphicsPipelineObject.hpp"
#include "Clove/Graphics/Vulkan/VulkanPipelineObject.hpp"
#include "Clove/Graphics/Vulkan/VulkanSemaphore.hpp"
#include "Clove/Graphics/Vulkan/VulkanTransferCommandBuffer.hpp"

#include <Clove/Cast.hpp>
#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    VulkanTransferQueue::VulkanTransferQueue(CommandQueueDescriptor descriptor, DevicePointer device, VkQueue queue, VkCommandPool commandPool, QueueFamilyIndices queueFamilyIndices)
        : descriptor{ descriptor }
        , device{ std::move(device) }
        , queue{ queue }
        , commandPool{ commandPool }
        , queueFamilyIndices{ queueFamilyIndices } {
    }

    VulkanTransferQueue::VulkanTransferQueue(VulkanTransferQueue &&other) noexcept = default;

    VulkanTransferQueue &VulkanTransferQueue::operator=(VulkanTransferQueue &&other) noexcept = default;

    VulkanTransferQueue::~VulkanTransferQueue() {
        vkDestroyCommandPool(device.get(), commandPool, nullptr);
    }

    CommandQueueDescriptor const &VulkanTransferQueue::getDescriptor() const {
        return descriptor;
    }

    std::unique_ptr<GhaTransferCommandBuffer> VulkanTransferQueue::allocateCommandBuffer() {
        //TODO: Multiple command buffer allocation

        VkCommandBuffer commandBuffer{ nullptr };

        VkCommandBufferAllocateInfo allocInfo{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = commandPool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        if(vkAllocateCommandBuffers(device.get(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Failed to allocate command buffer");
            return nullptr;
        }

        return createGhaObject<VulkanTransferCommandBuffer>(commandBuffer, queueFamilyIndices);
    }

    void VulkanTransferQueue::freeCommandBuffer(GhaTransferCommandBuffer &buffer) {
        VkCommandBuffer const vkbuffer{ polyCast<VulkanTransferCommandBuffer>(&buffer)->getCommandBuffer() };

        vkFreeCommandBuffers(device.get(), commandPool, 1, &vkbuffer);
    }

    void VulkanTransferQueue::submit(TransferSubmitInfo const &submission, GhaFence *signalFence) {
        //Wait semaphores / stages
        std::vector<VkSemaphore> waitSemaphores{};
        std::vector<VkPipelineStageFlags> waitStages{};
        size_t const waitSemaphoreCount{ submission.waitSemaphores.size() };
        waitSemaphores.resize(waitSemaphoreCount);
        waitStages.resize(waitSemaphoreCount);

        for(size_t i{ 0 }; i < waitSemaphoreCount; ++i) {
            waitSemaphores[i] = polyCast<VulkanSemaphore const>(submission.waitSemaphores[i].first)->getSemaphore();
            waitStages[i]     = convertStage(submission.waitSemaphores[i].second);
        }

        //Command buffers
        std::vector<VkCommandBuffer> commandBuffers{};
        size_t const commandBufferCount{ submission.commandBuffers.size() };
        commandBuffers.resize(commandBufferCount);
        for(size_t i{ 0 }; i < commandBufferCount; ++i) {
            commandBuffers[i] = polyCast<VulkanTransferCommandBuffer const>(submission.commandBuffers[i])->getCommandBuffer();
        }

        //Signal semaphores
        std::vector<VkSemaphore> signalSemaphores{};
        size_t const signalSemaphoreCount{ submission.signalSemaphores.size() };
        signalSemaphores.resize(signalSemaphoreCount);
        for(size_t i{ 0 }; i < signalSemaphoreCount; ++i) {
            signalSemaphores[i] = polyCast<VulkanSemaphore const>(submission.signalSemaphores[i])->getSemaphore();
        }

        VkSubmitInfo const submitInfo{
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount   = static_cast<uint32_t>(waitSemaphoreCount),
            .pWaitSemaphores      = waitSemaphores.data(),
            .pWaitDstStageMask    = waitStages.data(),
            .commandBufferCount   = static_cast<uint32_t>(commandBufferCount),
            .pCommandBuffers      = commandBuffers.data(),
            .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphoreCount),
            .pSignalSemaphores    = signalSemaphores.data(),
        };

        VkFence const vkFence{ signalFence != nullptr ? polyCast<VulkanFence const>(signalFence)->getFence() : VK_NULL_HANDLE };

        uint32_t constexpr submitCount{ 1 };
        if(vkQueueSubmit(queue, submitCount, &submitInfo, vkFence) != VK_SUCCESS) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Failed to submit transfer command buffer(s)");
        }
    }
}
