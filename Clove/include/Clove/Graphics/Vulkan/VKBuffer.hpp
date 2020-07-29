#pragma once

#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

//TODO: Remove
#include <vulkan/vulkan.hpp>
#include <Root/Definitions.hpp>

namespace clv::gfx {
    using UsageType = uint8_t;
    enum class BufferUsageMode : UsageType {
        TransferSource      = 1 << 0,
        TransferDestination = 1 << 1,
        VertexBuffer        = 1 << 2,
        IndexBuffer         = 1 << 3,
        UniformBuffer       = 1 << 4,
    };
    GARLIC_ENUM_BIT_FLAG_OPERATORS(BufferUsageMode, UsageType)

    struct BufferDescriptor2 {//TODO: Remove 2, this is because it is conflicting the previously defined type
        size_t size = 0;
        BufferUsageMode usageFlags;
        SharingMode sharingMode;
        MemoryType memoryType;
    };
}

namespace clv::gfx::vk {
    class VKBuffer {
        //VARIABLES
    private:
        VkDevice device = VK_NULL_HANDLE;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory bufferMemory;

        BufferDescriptor2 descriptor;

        //FUNCTIONS
    public:
        //TODO: Ctors
        VKBuffer() = delete;
        VKBuffer(VkDevice device, VkPhysicalDevice physicalDevice, BufferDescriptor2 descriptor, const QueueFamilyIndices& familyIndices);

        ~VKBuffer();

        void map(const void* data, const size_t size);

        VkBuffer getBuffer() const;
    };
}