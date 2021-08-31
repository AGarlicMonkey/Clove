#pragma once

#include "Clove/Graphics/GhaComputeQueue.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanComputeQueue : public GhaComputeQueue {
        //VARIABLES
    private:
        CommandQueueDescriptor descriptor{};

        DevicePointer device;
        VkQueue queue{ VK_NULL_HANDLE };
        VkCommandPool commandPool{ VK_NULL_HANDLE };

        QueueFamilyIndices queueFamilyIndices;

        //FUNCTIONS
    public:
        VulkanComputeQueue() = delete;
        VulkanComputeQueue(CommandQueueDescriptor descriptor, DevicePointer device, VkQueue queue, VkCommandPool commandPool, QueueFamilyIndices queueFamilyIndices);

        VulkanComputeQueue(VulkanComputeQueue const &other) = delete;
        VulkanComputeQueue(VulkanComputeQueue &&other) noexcept;

        VulkanComputeQueue &operator=(VulkanComputeQueue const &other) = delete;
        VulkanComputeQueue &operator=(VulkanComputeQueue &&other) noexcept;

        ~VulkanComputeQueue();

        CommandQueueDescriptor const &getDescriptor() const override;

        std::unique_ptr<GhaComputeCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(std::unique_ptr<GhaComputeCommandBuffer> &buffer) override;

        void submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
