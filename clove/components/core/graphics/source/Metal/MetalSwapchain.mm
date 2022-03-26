#include "Clove/Graphics/Metal/MetalSwapchain.hpp"

#include "Clove/Graphics/Metal/MetalView.hpp"
#include "Clove/Graphics/Metal/MetalImage.hpp"
#include "Clove/Graphics/Metal/MetalImageView.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalSwapchain::MetalSwapchain(MetalView *view, id<MTLCommandQueue> presentQueue, std::vector<std::unique_ptr<MetalImage>> images, GhaImage::Format imageFormat, vec2ui imageSize)
        : view{ view }
        , presentQueue{ presentQueue }
        , images{ std::move(images) }
        , imageFormat{ imageFormat }
        , imageSize{ imageSize } {
        for(size_t i{ 0 }; i < this->images.size(); ++i) {
            imageQueue.push(i);
        }
    }
    
    MetalSwapchain::~MetalSwapchain() = default;
    
    std::pair<GhaImage *, Result> MetalSwapchain::aquireNextImage(GhaSemaphore const *availableSemaphore) {
        //TODO: Handle resizing
        
        if(imageQueue.empty()) {
            CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0} has no available images", CLOVE_FUNCTION_NAME_PRETTY);
            return { nullptr, Result::Unkown };
        }
        
        {
            std::scoped_lock lock{ imageQueueMutex };
            
            activeImage = imageQueue.front();
            imageQueue.pop();
        }
        
        //Because we're simulating the swapchain on the CPU side we need to signal the semaphores immediately.
        //This is a hack solution as we wouldn't want to just make an empty commit that only signals a
        //semaphore. But it'll do for now
        id<MTLCommandBuffer> commandBuffer{ [presentQueue commandBuffer] };
        id<MTLBlitCommandEncoder> encoder{ [commandBuffer blitCommandEncoder] };
        
        [encoder updateFence:polyCast<MetalSemaphore const>(availableSemaphore)->getFence()];
        [encoder endEncoding];
        
        [commandBuffer commit];
        
        return { images[activeImage].get(), Result::Success };
    }
    
    uint32_t MetalSwapchain::getCurrentImageIndex() const {
        return activeImage;
    }
    
    Result MetalSwapchain::present(std::vector<GhaSemaphore const *> waitSemaphores) {
        @autoreleasepool{
            id<MTLTexture> texture{ images[activeImage]->getTexture() };
            id<CAMetalDrawable> drawable{ view.metalLayer.nextDrawable };
            
            id<MTLCommandBuffer> commandBuffer{ [presentQueue commandBuffer] };
            id<MTLBlitCommandEncoder> encoder{ [commandBuffer blitCommandEncoder] };
            
            for(auto const &semaphore : waitSemaphores) {
                [encoder waitForFence:polyCast<MetalSemaphore const>(semaphore)->getFence()];
            }
            [encoder copyFromTexture:texture toTexture:drawable.texture];
            [encoder endEncoding];
            
            [commandBuffer presentDrawable:drawable];
            [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
                std::scoped_lock lock{ imageQueueMutex };
                imageQueue.push(activeImage); //Return the image back to the queue when the present buffer is done with it.
            }];
            [commandBuffer commit];

            return Result::Success;
        }
    }
    
    GhaImage::Format MetalSwapchain::getImageFormat() const {
        return imageFormat;
    }
    
    vec2ui MetalSwapchain::getSize() const {
        return imageSize;
    }
}
