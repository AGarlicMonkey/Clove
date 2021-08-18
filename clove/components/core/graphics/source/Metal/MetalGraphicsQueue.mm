#include "Clove/Graphics/Metal/MetalGraphicsQueue.hpp"

#include "Clove/Graphics/Metal/MetalGraphicsCommandBuffer.hpp"
#include "Clove/Graphics/Metal/MetalView.hpp"
#include "Clove/Graphics/Metal/MetalFence.hpp"
#include "Clove/Graphics/Metal/MetalPipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalGraphicsQueue::MetalGraphicsQueue(clove::CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue)
        : commandQueue{ commandQueue } {
        allowBufferReuse = (descriptor.flags & QueueFlags::ReuseBuffers) != 0;
    }
    
    MetalGraphicsQueue::MetalGraphicsQueue(MetalGraphicsQueue &&other) noexcept = default;
    
    MetalGraphicsQueue& MetalGraphicsQueue::operator=(MetalGraphicsQueue &&other) noexcept = default;
    
    MetalGraphicsQueue::~MetalGraphicsQueue() = default;
    
    std::unique_ptr<GhaGraphicsCommandBuffer> MetalGraphicsQueue::allocateCommandBuffer() {
        return std::make_unique<MetalGraphicsCommandBuffer>(allowBufferReuse);
    }
    
    void MetalGraphicsQueue::freeCommandBuffer(GhaGraphicsCommandBuffer &buffer) {
        //no op
    }
    
    void MetalGraphicsQueue::submit(std::vector<GraphicsSubmitInfo> const &submissions, GhaFence *signalFence) {
        @autoreleasepool{
            for(size_t i{ 0 }; i < submissions.size(); ++i) {
                auto const &submission{ submissions[i] };
                bool const isLastSubmission{ i == submissions.size() - 1 };
                
                for(auto const &commandBuffer : submission.commandBuffers) {
                    bool const isLastCommandBuffer{ commandBuffer == submission.commandBuffers.back() };
                    
                    auto *metalCommandBuffer{ polyCast<MetalGraphicsCommandBuffer>(commandBuffer.get()) };
                    if(metalCommandBuffer == nullptr) {
                        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: Command buffer provided is nullptr", CLOVE_FUNCTION_NAME);
                        continue;
                    }
                    
                    if(metalCommandBuffer->getCommandBufferUsage() == CommandBufferUsage::OneTimeSubmit && metalCommandBuffer->bufferHasBeenUsed()){
                        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "GraphicsCommandBuffer recorded with CommandBufferUsage::OneTimeSubmit has already been used. Only buffers recorded with CommandBufferUsage::Default can submitted multiples times after being recorded once.");
                        break;
                    }
                    
                    id<MTLCommandBuffer> executionBuffer{ [commandQueue commandBuffer] };
                    
                    for(auto const &pass : metalCommandBuffer->getEncodedRenderPasses()) {
                        id<MTLRenderCommandEncoder> encoder{ pass.begin(executionBuffer) };
                        
                        //Inject the wait semaphore into each buffer
                        for (auto const &semaphore : submission.waitSemaphores) {
                            auto const *metalSemaphore{ polyCast<MetalSemaphore const>(semaphore.first.get()) };
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
                                [encoder updateFence:polyCast<MetalSemaphore const>(semaphore.get())->getFence()
                                         afterStages:MTLRenderStageFragment];
                            }
                        }
                        
                        [encoder endEncoding];
                    }
                    
                    //For the last submission signal the fence
                    if(isLastSubmission && signalFence != nullptr) {
                        __block auto *fence{ polyCast<MetalFence>(signalFence) };
                        [executionBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
                            fence->signal();
                        }];
                    }
                    
                    [executionBuffer commit];
                    
                    metalCommandBuffer->markAsUsed();
                }
            }
        }
    }
}
