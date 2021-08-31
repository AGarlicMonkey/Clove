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
        struct Header {
            size_t blockSize{ 0 };
        };

        //VARIABLES
    private:
        std::byte *rawList{ nullptr }; /**< The raw list to allocate from if there are no free blocks. */
        size_t listSize{ 0 };

        std::byte *head{ nullptr };

        std::list<Header *> freeList; /**< Keeps track of any previously allocated blocks that are now free. */

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