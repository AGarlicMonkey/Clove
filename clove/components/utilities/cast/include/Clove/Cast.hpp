#pragma once

namespace clove {
    /**
     * @brief Casts a polymorphic type from SourceType to DestType.
     * @details This cast is designed for situations where you know
     * it is okay to perform a static_cast on a base type. If the
     * CMake flag 'CLOVE_USE_SAFE_CAST' is set to on then this will
     * dynamic_cast instead. This is useful in debug builds where
     * safety and error catching is important but still allows for
     * non debug builds to retain the speed of not constantly looking
     * up rtti infomation.
     */
    template<typename DestType, typename SourceType>
    DestType *polyCast(SourceType *source);
}

#include "Cast.inl"