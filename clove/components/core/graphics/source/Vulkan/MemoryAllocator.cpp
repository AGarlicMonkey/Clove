#include "Clove/Graphics/Vulkan/MemoryAllocator.hpp"

#include "Clove/Graphics/Vulkan/VulkanLog.hpp"

namespace clove {
    namespace {
        uint32_t getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
            VkPhysicalDeviceMemoryProperties memoryType{};
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryType);

            for(uint32_t i = 0; i < memoryType.memoryTypeCount; ++i) {
                if((typeBits & (1 << i)) != 0 && (memoryType.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }

            CLOVE_ASSERT_MSG(false, "{0}: Failed to find the specified index", CLOVE_FUNCTION_NAME);
            return -1;
        }
    }

    MemoryAllocator::Block::Block(VkDevice device, VkDeviceMemory memory, VkDeviceSize size, uint32_t memoryTypeIndex)
        : device{ device }
        , memory{ memory }
        , size{ size }
        , memoryTypeIndex{ memoryTypeIndex } {
        chunks.emplace_back(0, size, memory);
    }

    MemoryAllocator::Block::Block(Block &&other) noexcept
        : device{ other.device }
        , memory{ other.memory }
        , size{ other.size }
        , memoryTypeIndex{ other.memoryTypeIndex }
        , chunks{ std::move(other.chunks) } {
        other.memory = VK_NULL_HANDLE;//Make sure the moved block no longer points to our memory
    }

    MemoryAllocator::Block &MemoryAllocator::Block::operator=(Block &&other) noexcept {
        device          = other.device;
        memory          = other.memory;
        size            = other.size;
        memoryTypeIndex = other.memoryTypeIndex;
        chunks          = std::move(other.chunks);

        other.memory = VK_NULL_HANDLE;//Make sure the moved block no longer points to our memory

        return *this;
    }

    MemoryAllocator::Block::~Block() {
        vkFreeMemory(device, memory, nullptr);
    }

    MemoryAllocator::Chunk const *MemoryAllocator::Block::allocate(VkDeviceSize const size, VkDeviceSize const alignment) {
        for(auto chunk = std::begin(chunks); chunk != std::end(chunks); ++chunk) {
            if(chunk->free) {
                VkDeviceSize const remainingAlignment{ chunk->offset % alignment };                                    //How much we're off from the alignment
                VkDeviceSize const alignmentCorrection{ remainingAlignment != 0 ? alignment - remainingAlignment : 0 };//How much we compensate for to achieve a multiple of alignment

                if(VkDeviceSize const chunkSize = chunk->size - alignmentCorrection; chunkSize >= size) {
                    auto const prevOffset = chunk->offset;

                    chunk->offset += alignmentCorrection;
                    chunk->size = chunkSize;
                    chunk->free = false;

                    //Add back the extra bits to the chunk to the left
                    if(auto const extraBits{ chunk->offset - prevOffset }; extraBits > 0) {
                        std::prev(chunk)->size += extraBits;
                    }

                    //If we have room left in the chunk, split it and put the excess back in the list.
                    if(VkDeviceSize const remainingSize{ chunk->size - size }; remainingSize > 0) {
                        chunks.insert(std::next(chunk), Chunk(chunk->offset + size, remainingSize, memory));
                        chunk->size = size;
                    }

                    return &*chunk;
                }
            }
        }

        return nullptr;
    }

    bool MemoryAllocator::Block::free(Chunk const *&chunkPtr) {
        for(auto chunk = std::begin(chunks); chunk != std::end(chunks); ++chunk) {
            if(*chunk == *chunkPtr) {
                chunk->free = true;

                bool removeIndex{ false };
                bool removeRight{ false };

                //Merge neighbouring chunks
                if(auto rightChunk = std::next(chunk); rightChunk != std::end(chunks) && rightChunk->free) {
                    //Merge right into us
                    chunk->size += rightChunk->size;

                    removeRight = true;
                }
                if(chunk != std::begin(chunks)) {
                    if(auto leftChunk = std::prev(chunk); leftChunk->free) {
                        //Merge ourselves into left
                        leftChunk->size += chunk->size;

                        removeIndex = true;
                    }
                }

                //Remove the right most first to preserve indices
                if(removeRight) {
                    chunks.erase(std::next(chunk));
                }
                if(removeIndex) {
                    chunks.erase(chunk);
                }

                chunkPtr = nullptr;
                return true;
            }
        }

        return false;
    }

    MemoryAllocator::MemoryAllocator(DevicePointer device)
        : device{ std::move(device) } {
    }

    MemoryAllocator::MemoryAllocator(MemoryAllocator &&other) noexcept = default;

    MemoryAllocator &MemoryAllocator::operator=(MemoryAllocator &&other) noexcept = default;

    MemoryAllocator::~MemoryAllocator() = default;

    MemoryAllocator::Chunk const *MemoryAllocator::allocate(VkMemoryRequirements const &memoryRequirements, VkMemoryPropertyFlags properties) {
        uint32_t const memoryTypeIndex{ getMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties, device.getPhysical()) };

        Chunk const *freeChunk{ nullptr };
        for(auto &block : memoryBlocks) {
            if(block.getMemoryTypeIndex() == memoryTypeIndex) {
                freeChunk = block.allocate(memoryRequirements.size, memoryRequirements.alignment);

                if(freeChunk != nullptr) {
                    break;
                }
            }
        }

        if(freeChunk == nullptr) {
            //Make sure if allocate a new block that's big enough
            VkDeviceSize const size{ std::max(memoryRequirements.size, blockSize) };

            VkMemoryAllocateInfo const info{
                .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .pNext           = nullptr,
                .allocationSize  = size,
                .memoryTypeIndex = memoryTypeIndex,
            };

            VkDeviceMemory memory{ VK_NULL_HANDLE };
            if(VkResult const result{ vkAllocateMemory(device.get(), &info, nullptr, &memory) }; result != VK_SUCCESS) {
                switch(result) {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Could not allocate memory. Out of host memory.");
                        break;
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Could not allocate memory. Out of device memory.");
                        break;
                    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                        CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Could not allocate memory. Invalid External handle.");
                        break;
                    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR:
                        CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Could not allocate memory. Invalid opaque capture address.");
                        break;
                    default:
                        CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Could not allocate memory. Reason Unknown.");
                        break;
                }

                return nullptr;
            }

#if CLOVE_GHA_MEMORY_DEBUG
            VkPhysicalDeviceMemoryBudgetPropertiesEXT memoryBudgetProperties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT,
            };
            VkPhysicalDeviceMemoryProperties2 properties{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
                .pNext = &memoryBudgetProperties,
            };
            vkGetPhysicalDeviceMemoryProperties2(device.getPhysical(), &properties);

            uint32_t const heapIndex{ properties.memoryProperties.memoryTypes[memoryTypeIndex].heapIndex };

            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "Allocated block of {0} bytes from heap {1}. Current heap usage: {2} / {3}", size, heapIndex, memoryBudgetProperties.heapUsage[heapIndex], memoryBudgetProperties.heapBudget[heapIndex]);
#endif

            memoryBlocks.emplace_back(device.get(), memory, size, memoryTypeIndex);
            freeChunk = memoryBlocks.back().allocate(memoryRequirements.size, memoryRequirements.alignment);

            CLOVE_ASSERT_MSG(freeChunk != nullptr, "{0}: Newly allocated Block does not have enough room", CLOVE_FUNCTION_NAME_PRETTY);
        }

        return freeChunk;
    }

    void MemoryAllocator::free(Chunk const *&chunk) {
        for(auto &block : memoryBlocks) {
            if(block.free(chunk)) {
                break;
            }
        }
    }
}