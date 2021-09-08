#if CLOVE_GHA_VALIDATION
    #include "Clove/Graphics/Validation/ValidationBuffer.hpp"
    #include "Clove/Graphics/Validation/ValidationCommandBuffer.hpp"
    #include "Clove/Graphics/Validation/ValidationQueue.hpp"
    #include "Clove/Graphics/Validation/ValidationDescriptorPool.hpp"

    #include <type_traits>
#endif

namespace clove {
    template<typename GhaObjectType, typename... Args>
    std::unique_ptr<GhaObjectType> createGhaObject(Args &&...args) {
#if CLOVE_GHA_VALIDATION
        if constexpr(std::is_base_of_v<GhaBuffer, GhaObjectType>) {
            return std::make_unique<ValidationBuffer<GhaObjectType>>(std::forward<Args>(args)...);
        } else if constexpr(std::is_base_of_v<GhaGraphicsQueue, GhaObjectType>) {
            return std::make_unique<ValidationGraphicsQueue<GhaObjectType>>(std::forward<Args>(args)...);
        } else if constexpr(std::is_base_of_v<GhaComputeQueue, GhaObjectType>) {
            return std::make_unique<ValidationComputeQueue<GhaObjectType>>(std::forward<Args>(args)...);
        } else if constexpr(std::is_base_of_v<GhaTransferQueue, GhaObjectType>) {
            return std::make_unique<ValidationTransferQueue<GhaObjectType>>(std::forward<Args>(args)...);
        } else if constexpr(std::is_base_of_v<GhaGraphicsCommandBuffer, GhaObjectType>) {
            return std::make_unique<ValidationGraphicsCommandBuffer<GhaObjectType>>(std::forward<Args>(args)...);
        } else if constexpr(std::is_base_of_v<GhaComputeCommandBuffer, GhaObjectType>) {
            return std::make_unique<ValidationComputeCommandBuffer<GhaObjectType>>(std::forward<Args>(args)...);
        } else if constexpr(std::is_base_of_v<GhaTransferCommandBuffer, GhaObjectType>) {
            return std::make_unique<ValidationTransferCommandBuffer<GhaObjectType>>(std::forward<Args>(args)...);
        } else if constexpr(std::is_base_of_v<GhaDescriptorPool, GhaObjectType>) {
            return std::make_unique<ValidationDescriptorPool<GhaObjectType>>(std::forward<Args>(args)...);
        } else {
            return std::make_unique<GhaObjectType>(std::forward<Args>(args)...);
        }
#else
        return std::make_unique<GhaObjectType>(std::forward<Args>(args)...);
#endif
    }
}