#include "Clove/Graphics/Metal/MetalRenderPass.hpp"

namespace clove {
    MetalRenderPass::MetalRenderPass(MTLRenderPipelineColorAttachmentDescriptorArray* colourAttachments, MTLPixelFormat depthPixelFormat, Descriptor descriptor)
        : colourAttachments{ colourAttachments }
        , depthPixelFormat{ depthPixelFormat }
        , descriptor{ std::move(descriptor) }{
    }
    
    MetalRenderPass::MetalRenderPass(MetalRenderPass &&other) noexcept = default;
    
    MetalRenderPass& MetalRenderPass::operator=(MetalRenderPass &&other) noexcept = default;
    
    MetalRenderPass::~MetalRenderPass() = default;
}
