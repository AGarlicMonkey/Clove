#include "Clove/Graphics/Metal/MetalTransferCommandBuffer.hpp"

#include "Clove/Graphics/Metal/MetalBuffer.hpp"
#include "Clove/Graphics/Metal/MetalImage.hpp"
#include "Clove/Graphics/Metal/MetalPipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"

#include <Clove/Cast.hpp>

namespace clove {
	namespace {
		size_t getBytesPerPixel(GhaImage::Format imageFormat) {
			switch (imageFormat) {
				case GhaImage::Format::R8_UNORM:
					return 1;
				case GhaImage::Format::R8G8B8A8_SRGB:
				case GhaImage::Format::B8G8R8A8_SRGB:
				case GhaImage::Format::B8G8R8A8_UNORM:
				case GhaImage::Format::D32_SFLOAT:
					return 4;
				case GhaImage::Format::Unkown:
				default:
					CLOVE_ASSERT_MSG(false, "Unknown format type");
					return 0;
			}
		}
	}
	
	MetalTransferCommandBuffer::MetalTransferCommandBuffer() = default;
	
	MetalTransferCommandBuffer::MetalTransferCommandBuffer(MetalTransferCommandBuffer &&other) noexcept = default;
	
	MetalTransferCommandBuffer &MetalTransferCommandBuffer::operator=(MetalTransferCommandBuffer &&other) noexcept = default;
	
	MetalTransferCommandBuffer::~MetalTransferCommandBuffer() = default;

	void MetalTransferCommandBuffer::beginRecording(CommandBufferUsage usageFlag) {
		commands.clear();
	}
	
	void MetalTransferCommandBuffer::endRecording() {
		//no op
	}

	void MetalTransferCommandBuffer::copyBufferToBuffer(GhaBuffer &source, size_t const sourceOffset, GhaBuffer &destination, size_t const destinationOffset, size_t const sizeBytes) {
		commands.emplace_back([source = &source, sourceOffset, destination = &destination, destinationOffset, sizeBytes](id<MTLBlitCommandEncoder> encoder){
			[encoder copyFromBuffer:polyCast<MetalBuffer>(source)->getBuffer()
					   sourceOffset:sourceOffset
						   toBuffer:polyCast<MetalBuffer>(destination)->getBuffer()
				  destinationOffset:destinationOffset
							   size:sizeBytes];
		});
	}
	
	void MetalTransferCommandBuffer::copyBufferToImage(GhaBuffer &source, size_t const sourceOffset, GhaImage &destination, vec3i const &destinationOffset, vec3ui const &destinationExtent, uint32_t const destinationBaseLayer, uint32_t const destinationLayerCount) {
		commands.emplace_back([source = &source, sourceOffset, destination = &destination, destinationOffset, destinationExtent, destinationBaseLayer, destinationLayerCount](id<MTLBlitCommandEncoder> encoder){
			auto const *const metalImage{ polyCast<MetalImage>(destination) };
            if(metalImage == nullptr){
                CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: Source image is nullptr", CLOVE_FUNCTION_NAME);
                return;
            }
            
			auto const &imageDescriptor{ metalImage->getDescriptor() };
			
			[encoder copyFromBuffer:polyCast<MetalBuffer>(source)->getBuffer()
					   sourceOffset:sourceOffset
				  sourceBytesPerRow:getBytesPerPixel(imageDescriptor.format) * imageDescriptor.dimensions.x //Always assume the bytes per row is the same as the image's width. Metal does not consider 0 tightly packed like Vulkan does
				sourceBytesPerImage:getBytesPerPixel(imageDescriptor.format) * imageDescriptor.dimensions.x * imageDescriptor.dimensions.y * destinationLayerCount
						 sourceSize:MTLSizeMake(destinationExtent.x, destinationExtent.y, 1)
						  toTexture:metalImage->getTexture()
				   destinationSlice:destinationBaseLayer
				   destinationLevel:0
				  destinationOrigin:MTLOriginMake(destinationOffset.x, destinationOffset.y, 0)];
		});
	}

	void MetalTransferCommandBuffer::copyImageToBuffer(GhaImage &source, vec3i const &sourceOffset, vec3ui const &sourceExtent, uint32_t const sourceBaseLayer, uint32_t const sourceLayerCount, GhaBuffer &destination, size_t const destinationOffset) {
		commands.emplace_back([source = &source, sourceOffset, sourceExtent, destination = &destination, destinationOffset, sourceBaseLayer, sourceLayerCount](id<MTLBlitCommandEncoder> encoder){
			auto const *const metalImage{ polyCast<MetalImage>(source) };
            if(metalImage == nullptr){
                CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: Source image is nullptr", CLOVE_FUNCTION_NAME);
                return;
            }
            
			auto const &imageDescriptor{ metalImage->getDescriptor() };
			
			[encoder copyFromTexture:metalImage->getTexture()
						 sourceSlice:sourceBaseLayer
						 sourceLevel:0
						sourceOrigin:MTLOriginMake(sourceOffset.x, sourceOffset.y, 0)
						  sourceSize:MTLSizeMake(sourceExtent.x, sourceExtent.y, 1)
							toBuffer:polyCast<MetalBuffer>(destination)->getBuffer()
				   destinationOffset:destinationOffset
			  destinationBytesPerRow:getBytesPerPixel(imageDescriptor.format) * imageDescriptor.dimensions.x //Always assume the bytes per row is the same as the image's width. Metal does not consider 0 tightly packed like Vulkan does
			destinationBytesPerImage:getBytesPerPixel(imageDescriptor.format) * imageDescriptor.dimensions.x * imageDescriptor.dimensions.y * sourceLayerCount];
		});
	}

	void MetalTransferCommandBuffer::bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
		//MTLBlitCommandEncoder does not have a barrier. This might not be an issue as a barrier will need to happen on the other queues for owner transfership
		//or will be synchronised with a semaphore / fence
	}
	
	void MetalTransferCommandBuffer::imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
		//MTLBlitCommandEncoder does not have a barrier. This might not be an issue as a barrier will need to happen on the other queues for owner transfership
		//or will be synchronised with a semaphore / fence
	}
}
