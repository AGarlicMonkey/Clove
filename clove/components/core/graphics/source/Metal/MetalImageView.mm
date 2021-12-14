#include "Clove/Graphics/Metal/MetalImageView.hpp"

#include <Clove/Log/Log.hpp>

namespace clove {
    MetalImageView::MetalImageView(GhaImage::Format viewedFormat, vec2ui viewedDimensions, id<MTLTexture> texture)
        : viewedFormat{ viewedFormat }
        , viewedDimensions{ viewedDimensions }
        , texture{ texture }{
    }
    
    MetalImageView::MetalImageView(MetalImageView &&other) noexcept = default;
    
    MetalImageView& MetalImageView::operator=(MetalImageView &&other) noexcept = default;
    
    MetalImageView::~MetalImageView() = default;
    
	GhaImage::Format MetalImageView::getImageFormat() const {
		return viewedFormat;
	}

    vec2ui const &MetalImageView::getImageDimensions() const {
		return viewedDimensions;
	}
	
    id<MTLTexture> MetalImageView::getTexture() const {
        return texture;
    }

	MTLTextureType MetalImageView::convertType(GhaImageView::Type type, uint32_t const arrayCount) {
		switch (type) {
            case GhaImageView::Type::_2D:
                if(arrayCount > 1) {
                    return MTLTextureType2DArray;
                } else {
                    return MTLTextureType2D;
                }
            case GhaImageView::Type::_3D:
                return MTLTextureType3D;
            case GhaImageView::Type::Cube:
                if(arrayCount > 1) {
                    return MTLTextureTypeCubeArray;
                } else {
                     return MTLTextureTypeCube;
                }
            default:
                CLOVE_ASSERT_MSG(false, "{0}: Unkown type passed", CLOVE_FUNCTION_NAME_PRETTY);
                return MTLTextureType2D;
        }
	}
}
