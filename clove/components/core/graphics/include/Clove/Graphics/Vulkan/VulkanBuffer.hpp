#pragma once

#include "Clove/Graphics/GhaBuffer.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"
#include "Clove/Graphics/Vulkan/MemoryAllocator.hpp"

#include <vulkan/vulkan.hpp>

namespace clove {
    struct QueueFamilyIndices;
}

namespace clove {
    class VulkanBuffer : public GhaBuffer {
        //VARIABLES
    private:
        DevicePointer device;

        VkBuffer buffer{ VK_NULL_HANDLE };

        Descriptor descriptor;

        std::shared_ptr<MemoryAllocator> memoryAllocator;
        MemoryAllocator::Chunk const *allocatedBlock{ nullptr };

        //FUNCTIONS
    public:
        VulkanBuffer() = delete;
        VulkanBuffer(DevicePointer device, VkBuffer buffer, Descriptor descriptor, std::shared_ptr<MemoryAllocator> memoryAllocator);

        VulkanBuffer(VulkanBuffer const &other) = delete;
        VulkanBuffer(VulkanBuffer &&other) noexcept;

        VulkanBuffer &operator=(VulkanBuffer const &other) = delete;
        VulkanBuffer &operator=(VulkanBuffer &&other) noexcept;

        ~VulkanBuffer();

        void write(void const *data, size_t const offset, size_t const size) override;

        void read(void *data, size_t const offset, size_t const size) override;

        VkBuffer getBuffer() const;
    };
}