#pragma once

#include "Clove/Graphics/GhaTransferQueue.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanTransferQueue : public GhaTransferQueue {
        //VARIABLES
    private:
        CommandQueueDescriptor descriptor{};

        DevicePointer device;
        VkQueue queue{ VK_NULL_HANDLE };
        VkCommandPool commandPool{ VK_NULL_HANDLE };

        QueueFamilyIndices queueFamilyIndices;

        //FUNCTIONS
    public:
        VulkanTransferQueue() = delete;
        VulkanTransferQueue(CommandQueueDescriptor descriptor, DevicePointer device, VkQueue queue, VkCommandPool commandPool, QueueFamilyIndices queueFamilyIndices);

        VulkanTransferQueue(VulkanTransferQueue const &other) = delete;
        VulkanTransferQueue(VulkanTransferQueue &&other) noexcept;

        VulkanTransferQueue &operator=(VulkanTransferQueue const &other) = delete;
        VulkanTransferQueue &operator=(VulkanTransferQueue &&other) noexcept;

        ~VulkanTransferQueue();

        CommandQueueDescriptor const &getDescriptor() const override;

        std::unique_ptr<GhaTransferCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(GhaTransferCommandBuffer &buffer) override;

        void submit(TransferSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
