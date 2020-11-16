#pragma once

#include <Root/Definitions.hpp>

namespace clv::gfx {
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
        TransferWrite               = 1 << 0,
        ShaderRead                  = 1 << 1,
        ColourAttachmentWrite       = 1 << 2,
        DepthStencilAttachmentRead  = 1 << 3,
        DepthStencilAttachmentWrite = 1 << 4
    };
    GARLIC_ENUM_BIT_FLAG_OPERATORS(AccessFlags, AccessFlagsType)
}