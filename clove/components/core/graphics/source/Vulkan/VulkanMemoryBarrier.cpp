#include "Clove/Graphics/Vulkan/VulkanMemoryBarrier.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>
#include <vulkan/vulkan.h>

namespace clove {
    uint32_t getQueueFamilyIndex(QueueType type, QueueFamilyIndices const &indices) {
        switch(type) {
            case QueueType::None:
                return VK_QUEUE_FAMILY_IGNORED;
            case QueueType::Graphics:
                return *indices.graphicsFamily;
            case QueueType::Transfer:
                return *indices.transferFamily;
            case QueueType::Compute:
                return *indices.computeFamily;
            default:
                CLOVE_ASSERT(false, "{0}: Unkown queue type", CLOVE_FUNCTION_NAME);
                return VK_QUEUE_FAMILY_IGNORED;
        }
    }
}
