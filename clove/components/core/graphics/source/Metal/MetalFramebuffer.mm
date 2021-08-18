#include "Clove/Graphics/Metal/MetalFramebuffer.hpp"

namespace clove {
    MetalFramebuffer::MetalFramebuffer(MTLRenderPassDescriptor *renderPassDescriptor)
        : renderPassDescriptor{ renderPassDescriptor } {
    }
    
    MetalFramebuffer::MetalFramebuffer(MetalFramebuffer &&other) noexcept = default;
    
    MetalFramebuffer& MetalFramebuffer::operator=(MetalFramebuffer &&other) noexcept = default;
    
    MetalFramebuffer::~MetalFramebuffer() = default;
    
    MTLRenderPassDescriptor *MetalFramebuffer::getRenderPassDescriptor() const {
        return renderPassDescriptor;
    }
}
