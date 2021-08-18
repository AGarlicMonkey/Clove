#pragma once

#include "AllocatorStrategy.hpp"

#include <cstddef>
#include <memory>

namespace clove {
    /**
     * @brief An allocator that prevents memory being fragmented but can only allocate a single type of item.
     * @details A PoolAllocator can only work on a single type of item because internally it works like 
     * a linked list, inserting new items into free spaces. A PoolAllocator is less flexible than a
     * ListAllocator because it can only allocate a single item but can very easily avoid memory fragmentation.
     */
    template<typename ItemType, AllocatorStrategy strategy = AllocatorStrategy::Fixed>
    class PoolAllocator {
        //TYPES
    private:
        union PoolItem {
            PoolItem *next;
            alignas(alignof(ItemType)) char item[sizeof(ItemType)];
        };

        struct PoolArena {
            //VARIABLES
        public:
            std::unique_ptr<PoolItem[]> storage;
            std::unique_ptr<PoolArena> next;

            //FUNCTIONS
        public:
            PoolArena(size_t const itemCount);
        };

        //VARIABLES
    private:
        size_t elementsPerArena{ 0 };

        std::unique_ptr<PoolArena> arena;
        PoolItem *nextFree{ nullptr };

        //FUNCTIONS
    public:
        PoolAllocator() = delete;
        PoolAllocator(size_t const elementsPerArena);

        PoolAllocator(PoolAllocator const &other) = delete;
        PoolAllocator(PoolAllocator &&other) noexcept;

        PoolAllocator &operator=(PoolAllocator const &other) = delete;
        PoolAllocator &operator=(PoolAllocator &&other) noexcept;

        ~PoolAllocator();

        /**
         * @brief Allocates a new item of ItemType. Calling it's constructor.
         */
        template<typename... Args>
        ItemType *alloc(Args &&... args);

        /**
         * @brief Frees an item, calling it's destructor.
         */
        void free(ItemType *item);
    };
}

#include "PoolAllocator.inl"