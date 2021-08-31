#include "Clove/Graphics/Metal/MetalGraphicsQueue.hpp"

#include "Clove/Graphics/Helpers.hpp"
#include "Clove/Graphics/Metal/MetalGraphicsCommandBuffer.hpp"
#include "Clove/Graphics/Metal/MetalView.hpp"
#include "Clove/Graphics/Metal/MetalFence.hpp"
#include "Clove/Graphics/Metal/MetalPipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalGraphicsQueue::MetalGraphicsQueue(CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue)
        : descriptor{ descriptor }
        , commandQueue{ commandQueue } {
    }
    
    MetalGraphicsQueue::MetalGraphicsQueue(MetalGraphicsQueue &&other) noexcept = default;
    
    MetalGraphicsQueue& MetalGraphicsQueue::operator=(MetalGraphicsQueue &&other) noexcept = default;
    
    MetalGraphicsQueue::~MetalGraphicsQueue() = default;

    CommandQueueDescriptor const &MetalGraphicsQueue::getDescriptor() const {
        return descriptor;
    }
    
    std::unique_ptr<GhaGraphicsCommandBuffer> MetalGraphicsQueue::allocateCommandBuffer() {
        return createGhaObject<MetalGraphicsCommandBuffer>();
    }
    
    void MetalGraphicsQueue::freeCommandBuffer(std::unique_ptr<GhaGraphicsCommandBuffer> &buffer) {
        buffer.reset();
    }
    
    void MetalGraphicsQueue::submit(GraphicsSubmitInfo const &submission, GhaFence *signalFence) {
        @autoreleasepool{
            for(auto *commandBuffer : submission.commandBuffers) {
                bool const isLastCommandBuffer{ commandBuffer == submission.commandBuffers.back() };
                
                auto *metalCommandBuffer{ polyCast<MetalGraphicsCommandBuffer>(commandBuffer) };
                if(metalCommandBuffer == nullptr) {
                    CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: Command buffer provided is nullptr", CLOVE_FUNCTION_NAME);
                    continue;
                }
               
                id<MTLCommandBuffer> executionBuffer{ [commandQueue commandBuffer] };
                
                for(auto const &pass : metalCommandBuffer->getEncodedRenderPasses()) {
                    id<MTLRenderCommandEncoder> encoder{ pass.begin(executionBuffer) };
                    
                    //Inject the wait semaphore into each buffer
                    for (auto const &semaphore : submission.waitSemaphores) {
                        auto const *const metalSemaphore{ polyCast<MetalSemaphore const>(semaphore.first) };
                        if(metalSemaphore == nullptr) {
                            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: Semaphore provided is nullptr", CLOVE_FUNCTION_NAME);
                            continue;
                        }
                        
                        MTLRenderStages const waitStage{ convertStage(semaphore.second) };
                        
                        [encoder waitForFence:metalSemaphore->getFence()
                                 beforeStages:waitStage];
                    }
                    
                    //Excute all recorded commands for the encoder
                    for(auto const &command : pass.commands) {
                        command(encoder);
                    }
                    
                    //For the last buffer add all semaphore signalling
                    if(isLastCommandBuffer && !submission.signalSemaphores.empty()) {
                        for(auto const &semaphore : submission.signalSemaphores) {
                            [encoder updateFence:polyCast<MetalSemaphore const>(semaphore)->getFence()
                                     afterStages:MTLRenderStageFragment];
                        }
                    }
                    
                    [encoder endEncoding];
                }
                
                if(signalFence != nullptr) {
                    __block auto *fence{ polyCast<MetalFence>(signalFence) };
                    [executionBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
                        fence->signal();
                    }];
                }
                
                [executionBuffer commit];
            }
        }
    }
}
