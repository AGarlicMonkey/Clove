#include "Clove/Graphics/Metal/MetalTransferQueue.hpp"

#include "Clove/Graphics/Helpers.hpp"
#include "Clove/Graphics/Metal/MetalPipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalTransferCommandBuffer.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"
#include "Clove/Graphics/Metal/MetalFence.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalTransferQueue::MetalTransferQueue(id<MTLCommandQueue> commandQueue)
        : commandQueue{ commandQueue } {
    }
    
    MetalTransferQueue::MetalTransferQueue(MetalTransferQueue &&other) noexcept = default;
    
    MetalTransferQueue& MetalTransferQueue::operator=(MetalTransferQueue &&other) noexcept = default;
    
    MetalTransferQueue::~MetalTransferQueue() = default;

    std::unique_ptr<GhaTransferCommandBuffer> MetalTransferQueue::allocateCommandBuffer() {
        return createGhaObject<MetalTransferCommandBuffer>([commandQueue commandBuffer]);
    }
    
    void MetalTransferQueue::freeCommandBuffer(std::unique_ptr<GhaTransferCommandBuffer> &buffer) {
        buffer.reset();
    }
    
    void MetalTransferQueue::submit(TransferSubmitInfo const &submission, GhaFence *signalFence) {
        @autoreleasepool{
            for(auto *commandBuffer : submission.commandBuffers) {
                bool const isLastCommandBuffer{ commandBuffer == submission.commandBuffers.back() };
                
                auto *metalCommandBuffer{ polyCast<MetalTransferCommandBuffer>(commandBuffer) };
                if(metalCommandBuffer == nullptr) {
                    CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: Command buffer provided is nullptr", CLOVE_FUNCTION_NAME);
                    continue;
                }
                
                id<MTLCommandBuffer> executionBuffer{ metalCommandBuffer->getMtlCommandBuffer() };
                id<MTLBlitCommandEncoder> encoder{ metalCommandBuffer->getEncoder() };
                
                //Inject the wait semaphore into each buffer
                for (auto const &semaphore : submission.waitSemaphores) {
                    auto const *const metalSemaphore{ polyCast<MetalSemaphore const>(semaphore.first) };
                    if(metalSemaphore == nullptr) {
                        CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: Semaphore provided is nullptr", CLOVE_FUNCTION_NAME);
                        continue;
                    }
                    
                    [encoder waitForFence:metalSemaphore->getFence()];
                }
                
                //For the last buffer add all semaphore signalling
                if(isLastCommandBuffer) {
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
