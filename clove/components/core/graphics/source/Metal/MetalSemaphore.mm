#include "Clove/Graphics/Metal/MetalSemaphore.hpp"

namespace clove {
    MetalSemaphore::MetalSemaphore(id<MTLFence> fence)
        : fence{ fence } {
    }
    
    MetalSemaphore::MetalSemaphore(MetalSemaphore &&other) noexcept = default;
    
    MetalSemaphore &MetalSemaphore::operator=(MetalSemaphore &&other) noexcept = default;
    
    MetalSemaphore::~MetalSemaphore() = default;
}
