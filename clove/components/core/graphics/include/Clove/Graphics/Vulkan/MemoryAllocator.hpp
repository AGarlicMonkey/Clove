#pragma once

#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <list>
#include <vector>
#include <vulkan/vulkan.h>

namespace clove {
    /**
     * @brief Handles allocating memory from Vulkan's VkDeviceMemory. Aims to reduce memory fragmentation
     * and to reduce the amount of vkAllocateMemory calls.
     * @details MemoryAllocator pools blocks of memory of 256MB. It will sub-allocate
     * regions of those blocks as required, keeping the memory contiguous and byte aligned. If an allocation
     * is larger than 256MB then a new VkDeviceMemory is created of the required size
     */
    class MemoryAllocator {
        //TYPES
    public:
        /**
         * @brief A portion of allocated memory inside a VkDeviceMemory.
         */
        struct Chunk {
            VkDeviceSize offset{ 0 }; /**< Offset in to the memory this chunk occupies. */
            VkDeviceSize size{ 0 };   /**< Size of this memory chunk. */

            VkDeviceMemory memory{ VK_NULL_HANDLE };

            bool free{ true };

            Chunk(VkDeviceSize offset, VkDeviceSize size, VkDeviceMemory memory)
                : offset(offset)
                , size(size)
                , memory(memory) {
            }

            friend bool operator==(Chunk const &lhs, Chunk const &rhs) {
                return lhs.offset == rhs.offset &&
                    lhs.size == rhs.size &&
                    lhs.memory == rhs.memory &&
                    lhs.free == rhs.free;
            }
        };

    private:
        /**
         * @brief Represents a single VkDeviceMemory allocation.
         */
        class Block {
            //VARIABLES
        private:
            VkDevice device{ VK_NULL_HANDLE };

            VkDeviceMemory memory{ VK_NULL_HANDLE };
            VkDeviceSize size{ 0 };
            uint32_t memoryTypeIndex{ 0 };

            std::list<Chunk> chunks;

            //FUNCTIONS
        public:
            Block() = delete;
            Block(VkDevice device, VkDeviceSize size, uint32_t memoryTypeIndex);

            Block(Block const &other) = delete;
            Block(Block &&other) noexcept;

            Block &operator=(Block const &other) = delete;
            Block &operator=(Block &&other) noexcept;

            ~Block();

            Chunk const *allocate(VkDeviceSize const size, VkDeviceSize const alignment);
            bool free(Chunk const *&chunkPtr);

            inline VkDeviceSize getSize() const;

            inline uint32_t getMemoryTypeIndex() const;
        };

        //VARIABLES
    private:
        static VkDeviceSize constexpr blockSize = 256 * 1024 * 1024;//256MB

        DevicePointer device;

        std::vector<Block> memoryBlocks;

        //FUNCTIONS
    public:
        MemoryAllocator() = delete;
        MemoryAllocator(DevicePointer device);

        MemoryAllocator(MemoryAllocator const &other) = delete;
        MemoryAllocator(MemoryAllocator &&other) noexcept;

        MemoryAllocator &operator=(MemoryAllocator const &other) = delete;
        MemoryAllocator &operator=(MemoryAllocator &&other) noexcept;

        ~MemoryAllocator();

        Chunk const *allocate(VkMemoryRequirements const &memoryRequirements, VkMemoryPropertyFlags properties);
        /**
         * @brief Releases the memory the chunk occupies. Nulls the chunk pointer if successful
         */
        void free(Chunk const *&chunk);
    };
}

#include "MemoryAllocator.inl"