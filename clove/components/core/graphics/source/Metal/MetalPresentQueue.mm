#include "Clove/Graphics/Metal/MetalPresentQueue.hpp"

#include "Clove/Graphics/Metal/MetalView.hpp"
#include "Clove/Graphics/Metal/MetalSwapchain.hpp"
#include "Clove/Graphics/Metal/MetalImageView.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalPresentQueue::MetalPresentQueue(id<MTLCommandQueue> commandQueue, MetalView *view)
        : commandQueue{ commandQueue }
        , view{ view } {
    }
    
    MetalPresentQueue::MetalPresentQueue(MetalPresentQueue &&other) noexcept = default;
    
    MetalPresentQueue &MetalPresentQueue::operator=(MetalPresentQueue &&other) noexcept = default;
    
    MetalPresentQueue::~MetalPresentQueue() = default;
    
    Result MetalPresentQueue::present(PresentInfo const &presentInfo) {
        @autoreleasepool{
            MetalSwapchain *swapchain{ polyCast<MetalSwapchain>(presentInfo.swapChain.get()) };
            if(swapchain == nullptr) {
                //Upon receiving this error a new swapchain should be created. Hopefully fixing the nullptr
                return Result::Error_SwapchainOutOfDate;
            }
            
            id<MTLTexture> texture{ polyCast<MetalImageView>(swapchain->getImageViews()[presentInfo.imageIndex].get())->getTexture() };
            id<CAMetalDrawable> drawable{ view.metalLayer.nextDrawable };
            
            id<MTLCommandBuffer> commandBuffer{ [commandQueue commandBuffer] };
            
            id<MTLBlitCommandEncoder> encoder{ [commandBuffer blitCommandEncoder] };
            for(auto const &semaphore : presentInfo.waitSemaphores) {
                [encoder waitForFence:polyCast<MetalSemaphore const>(semaphore.get())->getFence()];
            }
            [encoder copyFromTexture:texture toTexture:drawable.texture];
            [encoder endEncoding];
            
            [commandBuffer presentDrawable:drawable];
            [commandBuffer commit];
            
            swapchain->markIndexAsFree(presentInfo.imageIndex);
            
            return Result::Success;
        }
    }
}
