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

	MTLTextureType MetalImageView::convertType(GhaImageView::Type const type) {
		switch (type) {
            case GhaImageView::Type::_2D:
                return MTLTextureType2D;
            case GhaImageView::Type::_2DArray:
                return MTLTextureType2DArray;
            case GhaImageView::Type::_3D:
                return MTLTextureType3D;
            case GhaImageView::Type::Cube:
                return MTLTextureTypeCube;
            case GhaImageView::Type::CubeArray:
                return MTLTextureTypeCubeArray;
            default:
                CLOVE_ASSERT_MSG(false, "{0}: Unkown type passed", CLOVE_FUNCTION_NAME_PRETTY);
                return MTLTextureType2D;
        }
	}
}
