#pragma once

namespace clove {
    enum class AllocatorStrategy {
        Fixed,   /**< Allocator is fixed sized, cannot allocate past initial size. */
        Dynamic, /**< Allocator can grow in size to accomdate new allocations. */
    };
}