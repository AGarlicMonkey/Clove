#include "Clove/Graphics/Metal/MetalComputeQueue.hpp"

#include "Clove/Graphics/Helpers.hpp"
#include "Clove/Graphics/Metal/MetalComputeCommandBuffer.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"
#include "Clove/Graphics/Metal/MetalFence.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalComputeQueue::MetalComputeQueue(CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue)
        : descriptor{ descriptor }
        , commandQueue{ commandQueue } {
    }
    
    MetalComputeQueue::MetalComputeQueue(MetalComputeQueue &&other) noexcept = default;
    
    MetalComputeQueue &MetalComputeQueue::operator=(MetalComputeQueue &&other) noexcept = default;
    
    MetalComputeQueue::~MetalComputeQueue() = default;

    CommandQueueDescriptor const &MetalComputeQueue::getDescriptor() const {
        return descriptor;
    }
    
    std::unique_ptr<GhaComputeCommandBuffer> MetalComputeQueue::allocateCommandBuffer() {
        return createGhaObject<MetalComputeCommandBuffer>();
    }
    
    void MetalComputeQueue::freeCommandBuffer(GhaComputeCommandBuffer &buffer) {
        //no op
    }
    
    void MetalComputeQueue::submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) {
        @autoreleasepool{
            for(auto *commandBuffer : submission.commandBuffers) {
                bool const isLastCommandBuffer{ commandBuffer == submission.commandBuffers.back() };
                
                auto *metalCommandBuffer{ polyCast<MetalComputeCommandBuffer>(commandBuffer) };
                if(metalCommandBuffer == nullptr) {
                    CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: Command buffer provided is nullptr", CLOVE_FUNCTION_NAME);
                    continue;
                }
                
                id<MTLCommandBuffer> executionBuffer{ [commandQueue commandBuffer] };
                id<MTLComputeCommandEncoder> encoder{ [executionBuffer computeCommandEncoder] };
                
                //Inject the wait semaphore into each buffer
                for (auto const &semaphore : submission.waitSemaphores) {
                    auto const *metalSemaphore{ polyCast<MetalSemaphore const>(semaphore.first) };
                    if(metalSemaphore == nullptr) {
                        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: Semaphore provided is nullptr", CLOVE_FUNCTION_NAME);
                        continue;
                    }
                    
                    [encoder waitForFence:metalSemaphore->getFence()];
                }
                
                //Excute all recorded commands for the encoder
                for(auto const &command : metalCommandBuffer->getCommands()) {
                    command(encoder);
                }
                
                //For the last buffer add all semaphore signalling
                if(isLastCommandBuffer && !submission.signalSemaphores.empty()) {
                    for(auto const &semaphore : submission.signalSemaphores) {
                        [encoder updateFence:polyCast<MetalSemaphore const>(semaphore)->getFence()];
                    }
                }
                
                if(signalFence != nullptr) {
                    __block auto *fence{ polyCast<MetalFence>(signalFence) };
                    [executionBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
                        fence->signal();
                    }];
                }
                
                [encoder endEncoding];
                [executionBuffer commit];
            }
        }
    }
}
