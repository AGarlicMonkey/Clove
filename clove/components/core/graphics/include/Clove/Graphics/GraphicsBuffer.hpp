#pragma once

#include "Clove/Graphics/GraphicsResource.hpp"

#include <Clove/Definitions.hpp>

namespace garlic::clove {
    /**
     * @brief Represents an arbitrary section of memory. Used in graphics operations.
     */
    class GraphicsBuffer {
        //TYPES
    public:
        using UsageModeType = uint8_t;
        enum class UsageMode : UsageModeType {
            TransferSource      = 1 << 0, /**< To be used as a source in a transfer operation. */
            TransferDestination = 1 << 1, /**< To be used as a destination in a transfer operation. */
            VertexBuffer        = 1 << 2,
            IndexBuffer         = 1 << 3,
            UniformBuffer       = 1 << 4,
        };

        struct Descriptor {
            size_t size{ 0 };
            UsageMode usageFlags;
            SharingMode sharingMode;
            MemoryType memoryType;
        };

        //FUNCTIONS
    public:
        virtual ~GraphicsBuffer() = default;

        /**
         * @brief Write data into the buffer. Requires the buffer to be created with MemoryType::SystemMemory.
         * @param data A pointer to the data that will be written to this buffer.
         * @param offset An offset into the buffer where to write the data to.
         * @param size Size of the region to write to.
         */
        virtual void write(void const *data, size_t const offset, size_t const size) = 0;

        /**
         * @brief Read a region of memory from the buffer. Requires the buffer to be created with MemoryType::SystemMemory.
         * @param data A caller owned pointer that will be written to with the read data.
         * @param offset An offset into the buffer where to read the data from.
         * @param size Size of the region to read from.
         */
        virtual void read(void *data, size_t const offset, size_t const size) = 0;
    };

    GARLIC_ENUM_BIT_FLAG_OPERATORS(GraphicsBuffer::UsageMode, GraphicsBuffer::UsageModeType)
}