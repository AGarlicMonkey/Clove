#pragma once

#include <cinttypes>
#include <cstddef>
#include <list>

namespace clove {
    /**
     * @brief An allocator that prevents memory fragmentation that works for any size of allocation.
     * @details A ListAllocator is a bit more complicated than a PoolAllocator because
     * it has to keep track of each block of free memory and how big that block is. It is
     * more difficult to completely avoid fragmentation but is more flexible because any
     * amount of memory can be allocated.
     */
    class ListAllocator {
        //TYPES
    private:
        struct Block {
            bool free{ true };
            size_t offset{ 0 };        /**< Offset into the backing memory of the block. */
            size_t alignedOffset{ 0 }; /**< The offset into the backing memory that respects the allocation's alignment. */
            size_t size{ 0 };          /**< Size of the entire block of memory. */
        };

        //VARIABLES
    private:
        std::byte *backingMemory{ nullptr }; /**< Underlying memory of the free list. */
        size_t backingMemorySize{ 0 };

        std::list<Block> freeList{}; /**< Current list of all free blocks on memory. */

        bool freeMemory{ true };

        //FUNCTIONS
    public:
        ListAllocator() = delete;
        ListAllocator(size_t sizeBytes);
        ListAllocator(std::byte *start, size_t sizeBytes);

        ListAllocator(ListAllocator const &other) = delete;
        ListAllocator(ListAllocator &&other) noexcept;

        ListAllocator &operator=(ListAllocator const &other) = delete;
        ListAllocator &operator=(ListAllocator &&other) noexcept;

        ~ListAllocator();

        /**
         * @brief Returns the total size that can be allocated.
         * @return 
         */
        inline size_t getSize() const;

        /**
         * @brief Returns a pointer to the memory in use by this allocator
         * @return 
         */
        inline std::byte const *getMemory() const;

        /**
         * @brief Allocates size amounts of bytes from the list.
         * @param size
         * @param alignment
         * @returns A Pointer to the allocated block of memory.
         */
        void *alloc(size_t size, size_t alignment);

        /**
         * @brief Allocate a block of memory for T.
         * @tparam T 
         * @return 
         */
        template<typename T>
        T *alloc();

        void free(void *ptr);
    };
}

#include "ListAllocator.inl"