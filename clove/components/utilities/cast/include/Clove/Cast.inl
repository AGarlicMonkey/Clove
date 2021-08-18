#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename DestType, typename SourceType>
    DestType *polyCast(SourceType *source) {
#if CLOVE_SAFE_CAST
        auto *result{ dynamic_cast<DestType *>(source) };
        CLOVE_ASSERT(result != nullptr, "Cast failed");
        return result;
#else
        return static_cast<DestType *>(source);
#endif
    }
}