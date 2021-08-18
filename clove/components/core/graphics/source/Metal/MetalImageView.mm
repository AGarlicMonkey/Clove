#include "Clove/Graphics/Metal/MetalImageView.hpp"

namespace clove {
    MetalImageView::MetalImageView(id<MTLTexture> texture)
        : texture{ texture }{
    }
    
    MetalImageView::MetalImageView(MetalImageView &&other) noexcept = default;
    
    MetalImageView& MetalImageView::operator=(MetalImageView &&other) noexcept = default;
    
    MetalImageView::~MetalImageView() = default;
    
    id<MTLTexture> MetalImageView::getTexture() const {
        return texture;
    }
}
