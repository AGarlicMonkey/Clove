#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename ItemType, AllocatorStrategy strategy>
    PoolAllocator<ItemType, strategy>::PoolArena::PoolArena(size_t const itemCount) {
        storage = std::unique_ptr<PoolItem[]>{ new PoolItem[itemCount] };
        for(size_t i = 1; i < itemCount; ++i) {
            storage[i - 1].next = &storage[i];
        }
        storage[itemCount - 1].next = nullptr;
    }

    template<typename ItemType, AllocatorStrategy strategy>
    PoolAllocator<ItemType, strategy>::PoolAllocator(size_t const elementsPerArena)
        : elementsPerArena{ elementsPerArena } {
        arena    = std::make_unique<PoolArena>(elementsPerArena);
        nextFree = &arena->storage[0];
    }

    template<typename ItemType, AllocatorStrategy strategy>
    PoolAllocator<ItemType, strategy>::PoolAllocator(PoolAllocator &&other) noexcept = default;

    template<typename ItemType, AllocatorStrategy strategy>
    PoolAllocator<ItemType, strategy> &PoolAllocator<ItemType, strategy>::operator=(PoolAllocator &&other) noexcept = default;

    template<typename ItemType, AllocatorStrategy strategy>
    PoolAllocator<ItemType, strategy>::~PoolAllocator() = default;

    template<typename ItemType, AllocatorStrategy strategy>
    template<typename... Args>
    ItemType *PoolAllocator<ItemType, strategy>::alloc(Args &&... args) {
        if(nextFree == nullptr) {
            if constexpr(strategy == AllocatorStrategy::Dynamic) {
                auto newArena{ std::make_unique<PoolArena>(elementsPerArena) };
                newArena->next = std::move(arena);
                arena          = std::move(newArena);
                nextFree       = &arena->storage[0];
            } else {
                CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: At the end of the free list. Cannot allocate new elements", CLOVE_FUNCTION_NAME_PRETTY);
                return nullptr;
            }
        }

        PoolItem *poolItem = nextFree;
        nextFree           = poolItem->next;

        ItemType *item = reinterpret_cast<ItemType *>(poolItem->item);
        new(item) ItemType(std::forward<Args>(args)...);

        return item;
    }

    template<typename ItemType, AllocatorStrategy strategy>
    void PoolAllocator<ItemType, strategy>::free(ItemType *item) {
        item->~ItemType();

        PoolItem *poolItem{ reinterpret_cast<PoolItem *>(item) };
        poolItem->next = nextFree;
        nextFree       = poolItem;
    }
}