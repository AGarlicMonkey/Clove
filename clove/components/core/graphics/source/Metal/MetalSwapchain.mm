#include "Clove/Graphics/Metal/MetalSwapchain.hpp"

#include "Clove/Graphics/Metal/MetalImage.hpp"
#include "Clove/Graphics/Metal/MetalImageView.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"

#include <Clove/Cast.hpp>

namespace clove {
	MetalSwapchain::MetalSwapchain(id<MTLCommandQueue> signalQueue, std::vector<std::unique_ptr<GhaImage>> images, GhaImage::Format imageFormat, vec2ui imageSize)
        : signalQueue{ signalQueue }
        , images{ std::move(images) }
		, imageFormat{ imageFormat }
		, imageSize{ imageSize } {
		imageViews.reserve(this->images.size());
			
		GhaImageView::Descriptor const viewDescriptor{
			.type = GhaImageView::Type::_2D,
		};
			
		for(size_t i{ 0 }; i < this->images.size(); ++i) {
			imageViews.emplace_back(this->images[i]->createView(viewDescriptor));
			imageQueue.push(i);
		}
	}
	
	MetalSwapchain::MetalSwapchain(MetalSwapchain &&other) noexcept = default;
	
	MetalSwapchain& MetalSwapchain::operator=(MetalSwapchain &&other) noexcept = default;
	
	MetalSwapchain::~MetalSwapchain() = default;
	
	std::pair<uint32_t, Result> MetalSwapchain::aquireNextImage(GhaSemaphore const *availableSemaphore) {
		//TODO: Handle resizing;
        
		if(imageQueue.empty()) {
			CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0} has no available images", CLOVE_FUNCTION_NAME_PRETTY);
			return { -1, Result::Unkown };
		}
		
		uint32_t const availableIndex{ imageQueue.front() };
		imageQueue.pop();
        
        //Because we're simulating the swapchain on the CPU side we need to signal the semaphores immediately.
        //This is a hack solution as we wouldn't want to just make an empty commit that only signals a
        //semaphore. But it'll do for now
        id<MTLCommandBuffer> commandBuffer{ [signalQueue commandBuffer] };
        id<MTLBlitCommandEncoder> encoder{ [commandBuffer blitCommandEncoder] };
        
        [encoder waitForFence:polyCast<MetalSemaphore const>(availableSemaphore)->getFence()];
        
        [encoder endEncoding];
        [commandBuffer commit];
        
		
		return { availableIndex, Result::Success };
	}

	GhaImage::Format MetalSwapchain::getImageFormat() const {
		return imageFormat;
	}
	
	vec2ui MetalSwapchain::getSize() const {
		return imageSize;
	}

	std::vector<GhaImageView *> MetalSwapchain::getImageViews() const {
        std::vector<GhaImageView *> views{};
        views.reserve(imageViews.size());
        
        for(auto const &view : imageViews) {
            views.push_back(view.get());
        }
        
		return views;
	}
	
	void MetalSwapchain::markIndexAsFree(uint32_t index) {
		imageQueue.push(index);
	}
}
