#pragma once

#include <Clove/Definitions.hpp>
#include <cinttypes>

namespace clove {
    enum class SharingMode {
        Exclusive, /**< Can only be accessed by a single queue type at a time. The ownership must be explicitly transfered between queues. */
        Concurrent /**< Can be accessed between different queue types without transferring ownership. */
    };

    enum class MemoryType {
        VideoMemory,  /**< Can't be written to be CPU, GPU optimised */
        SystemMemory, /**< Can be written to by CPU, not GPU optimised */
    };

    using AccessFlagsType = uint16_t;
    /**
     * @brief Memory access types that will participate in a memory dependency.
     */
    enum class AccessFlags : AccessFlagsType {
        None                        = 0,
        TransferRead                = 1 << 0,
        TransferWrite               = 1 << 1,
        ShaderRead                  = 1 << 2,
        ColourAttachmentWrite       = 1 << 3,
        DepthStencilAttachmentRead  = 1 << 4,
        DepthStencilAttachmentWrite = 1 << 5
    };
    CLOVE_ENUM_BIT_FLAG_OPERATORS(AccessFlags, AccessFlagsType)
}
