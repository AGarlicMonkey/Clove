#pragma once

#include <cstddef>

namespace clove {
    /**
     * @brief A memory allocator that is good for temporary allocations.
     * @details A StackAllocator allows any sized allocation but cannot free specific blocks of memory.
     * Instead you must either free the entire allocator or free everything beyond a certain marker.
     */
    class StackAllocator {
        //TYPES
    public:
        using Marker = size_t;

        //VARIABLES
    private:
        std::byte *stack{};
        size_t stackSize{ 0 };
        std::byte *top{ nullptr };

        bool freeMemory{ true };

        //FUNCTIONS
    public:
        StackAllocator() = delete;
        StackAllocator(size_t sizeBytes);
        StackAllocator(std::byte *start, size_t sizeBytes);

        StackAllocator(StackAllocator const &other) = delete;
        StackAllocator(StackAllocator &&other) noexcept;

        StackAllocator &operator=(StackAllocator const &other) = delete;
        StackAllocator &operator=(StackAllocator &&other) noexcept;

        ~StackAllocator();

        /**
         * @returns A marker of the current stack position of the allocator.
         */
        Marker markPosition();

        void *alloc(size_t bytes);

        /**
         * @brief Free the entire allocator, effectively resetting the stack.
         */
        void free();
        /**
         * @brief Free everything allocated after the marker.
         */
        void free(Marker marker);
    };
}