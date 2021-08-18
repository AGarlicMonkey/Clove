#include "Clove/Memory/StackAllocator.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>
#include <cstdlib>

namespace clove {
    StackAllocator::StackAllocator(size_t sizeBytes)
        : stackSize(sizeBytes) {
        stack = reinterpret_cast<std::byte *>(malloc(stackSize));
        top   = stack;
    }

    StackAllocator::StackAllocator(std::byte *start, size_t sizeBytes)
        : stackSize(sizeBytes)
        , freeMemory(false) {
        stack = start;
        top   = stack;
    }

    StackAllocator::StackAllocator(StackAllocator &&other) noexcept = default;

    StackAllocator &StackAllocator::operator=(StackAllocator &&other) noexcept = default;

    StackAllocator::~StackAllocator() {
        if(freeMemory) {
            ::free(stack);
        }
    }

    StackAllocator::Marker StackAllocator::markPosition() {
        return top - stack;
    }

    void *StackAllocator::alloc(size_t bytes) {
        if((top - stack) + bytes > stackSize) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: Not enough space left to allocate {1} bytes.", CLOVE_FUNCTION_NAME_PRETTY, bytes);
            return nullptr;
        }

        void *element = top;
        top += bytes;
        return element;
    }

    void StackAllocator::free() {
        top = stack;
    }

    void StackAllocator::free(Marker marker) {
        top = stack + marker;
    }
}