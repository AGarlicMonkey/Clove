#include "Clove/Graphics/Vulkan/VulkanComputeQueue.hpp"

#include "Clove/Graphics/Helpers.hpp"
#include "Clove/Graphics/Vulkan/VulkanComputeCommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanFence.hpp"
#include "Clove/Graphics/Vulkan/VulkanPipelineObject.hpp"
#include "Clove/Graphics/Vulkan/VulkanSemaphore.hpp"

#include <Clove/Cast.hpp>
#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    VulkanComputeQueue::VulkanComputeQueue(CommandQueueDescriptor descriptor, DevicePointer device, VkQueue queue, VkCommandPool commandPool, QueueFamilyIndices queueFamilyIndices)
        : descriptor{ descriptor }
        , device{ std::move(device) }
        , queue{ queue }
        , commandPool{ commandPool }
        , queueFamilyIndices{ queueFamilyIndices } {
    }

    VulkanComputeQueue::VulkanComputeQueue(VulkanComputeQueue &&other) noexcept = default;

    VulkanComputeQueue &VulkanComputeQueue::operator=(VulkanComputeQueue &&other) noexcept = default;

    VulkanComputeQueue::~VulkanComputeQueue() {
        vkDestroyCommandPool(device.get(), commandPool, nullptr);
    }

    CommandQueueDescriptor const &VulkanComputeQueue::getDescriptor() const {
        return descriptor;
    }

    std::unique_ptr<GhaComputeCommandBuffer> VulkanComputeQueue::allocateCommandBuffer() {
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

        return createGhaObject<VulkanComputeCommandBuffer>(commandBuffer, queueFamilyIndices);
    }

    void VulkanComputeQueue::freeCommandBuffer(GhaComputeCommandBuffer &buffer) {
        VkCommandBuffer const commandBuffer{ polyCast<VulkanComputeCommandBuffer>(&buffer)->getCommandBuffer() };
        vkFreeCommandBuffers(device.get(), commandPool, 1, &commandBuffer);
    }

    void VulkanComputeQueue::submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) {
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
            commandBuffers[i] = polyCast<VulkanComputeCommandBuffer const>(submission.commandBuffers[i])->getCommandBuffer();
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
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Failed to submit compute command buffer(s)");
        }
    }
}
