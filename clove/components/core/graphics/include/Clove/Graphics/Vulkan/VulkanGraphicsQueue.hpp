#pragma once

#include "Clove/Graphics/GhaGraphicsQueue.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanGraphicsQueue : public GhaGraphicsQueue {
        //VARIABLES
    private:
        CommandQueueDescriptor descriptor{};
        
        DevicePointer device;
        VkQueue queue{ VK_NULL_HANDLE };
        VkCommandPool commandPool{ VK_NULL_HANDLE };

        QueueFamilyIndices queueFamilyIndices;

        //FUNCTIONS
    public:
        VulkanGraphicsQueue() = delete;
        VulkanGraphicsQueue(CommandQueueDescriptor descriptor, DevicePointer device, VkQueue queue, VkCommandPool commandPool, QueueFamilyIndices queueFamilyIndices);

        VulkanGraphicsQueue(VulkanGraphicsQueue const &other) = delete;
        VulkanGraphicsQueue(VulkanGraphicsQueue &&other) noexcept;

        VulkanGraphicsQueue &operator=(VulkanGraphicsQueue const &other) = delete;
        VulkanGraphicsQueue &operator=(VulkanGraphicsQueue &&other) noexcept;

        ~VulkanGraphicsQueue();

        CommandQueueDescriptor const &getDescriptor() const override;

        std::unique_ptr<GhaGraphicsCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(std::unique_ptr<GhaGraphicsCommandBuffer> &buffer) override;

        void submit(GraphicsSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
