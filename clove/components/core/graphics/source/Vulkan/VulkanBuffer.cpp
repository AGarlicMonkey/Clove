#include "Clove/Graphics/Vulkan/VulkanBuffer.hpp"

#include "Clove/Graphics/Vulkan/VulkanResource.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <Clove/Log/Log.hpp>

namespace clove {
    namespace {
        VkMemoryPropertyFlags getMemoryPropertyFlags(MemoryType garlicProperties) {
            switch(garlicProperties) {
                case MemoryType::VideoMemory:
                    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                case MemoryType::SystemMemory:
                    return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;//Including HOST_COHERENT here as this makes mapping memory more simple
                default:
                    break;
            }
        }
    }

    VulkanBuffer::VulkanBuffer(DevicePointer device, VkBuffer buffer, Descriptor descriptor, std::shared_ptr<MemoryAllocator> memoryAllocator)
        : device{ std::move(device) }
        , buffer{ buffer }
        , descriptor{ descriptor }
        , memoryAllocator{ std::move(memoryAllocator) } {
        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(this->device.get(), buffer, &memoryRequirements);

        allocatedBlock = this->memoryAllocator->allocate(memoryRequirements, getMemoryPropertyFlags(this->descriptor.memoryType));

        vkBindBufferMemory(this->device.get(), buffer, allocatedBlock->memory, allocatedBlock->offset);
    }

    VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept = default;

    VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer &&other) noexcept = default;

    VulkanBuffer::~VulkanBuffer() {
        vkDestroyBuffer(device.get(), buffer, nullptr);
        memoryAllocator->free(allocatedBlock);
    }

    void VulkanBuffer::write(void const *data, size_t const offset, size_t const size) {
        CLOVE_ASSERT(descriptor.memoryType == MemoryType::SystemMemory, "{0}: Can only write to SystemMemory buffers", CLOVE_FUNCTION_NAME_PRETTY);

        void *cpuAccessibleMemory{ nullptr };

        vkMapMemory(device.get(), allocatedBlock->memory, allocatedBlock->offset + offset, size, 0, &cpuAccessibleMemory);
        memcpy(cpuAccessibleMemory, data, size);
        vkUnmapMemory(device.get(), allocatedBlock->memory);
    }

    void VulkanBuffer::read(void *data, size_t const offset, size_t const size) {
        CLOVE_ASSERT(descriptor.memoryType == MemoryType::SystemMemory, "{0}: Can only read from SystemMemory buffers", CLOVE_FUNCTION_NAME_PRETTY);

        void *cpuAccessibleMemory{ nullptr };

        vkMapMemory(device.get(), allocatedBlock->memory, allocatedBlock->offset + offset, size, 0, &cpuAccessibleMemory);
        memcpy(data, cpuAccessibleMemory, size);
        vkUnmapMemory(device.get(), allocatedBlock->memory);
    }

    VkBuffer VulkanBuffer::getBuffer() const {
        return buffer;
    }
}