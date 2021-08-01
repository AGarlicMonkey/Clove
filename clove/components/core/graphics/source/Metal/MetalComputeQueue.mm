#include "Clove/Graphics/Metal/MetalComputeQueue.hpp"

#include "Clove/Graphics/Metal/MetalComputeCommandBuffer.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"
#include "Clove/Graphics/Metal/MetalFence.hpp"

#include <Clove/Cast.hpp>

namespace garlic::clove {
    MetalComputeQueue::MetalComputeQueue(CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue)
        : commandQueue{ commandQueue } {
        allowBufferReuse = (descriptor.flags & QueueFlags::ReuseBuffers) != 0;
    }
    
    MetalComputeQueue::MetalComputeQueue(MetalComputeQueue &&other) noexcept = default;
    
    MetalComputeQueue &MetalComputeQueue::operator=(MetalComputeQueue &&other) noexcept = default;
    
    MetalComputeQueue::~MetalComputeQueue() = default;
    
    std::unique_ptr<GhaComputeCommandBuffer> MetalComputeQueue::allocateCommandBuffer() {
        return std::make_unique<MetalComputeCommandBuffer>(allowBufferReuse);
    }
    
    void MetalComputeQueue::freeCommandBuffer(GhaComputeCommandBuffer &buffer) {
        //no op
    }
    
    void MetalComputeQueue::submit(std::vector<ComputeSubmitInfo> const &submissions, GhaFence *signalFence) {
        @autoreleasepool{
            for(size_t i{ 0 }; i < submissions.size(); ++i) {
                auto const &submission{ submissions[i] };
                bool const isLastSubmission{ i == submissions.size() - 1 };
                
                for(auto const &commandBuffer : submission.commandBuffers) {
                    bool const isLastCommandBuffer{ commandBuffer == submission.commandBuffers.back() };
                    
                    auto *metalCommandBuffer{ polyCast<MetalComputeCommandBuffer>(commandBuffer.get()) };
                    if(metalCommandBuffer->getCommandBufferUsage() == CommandBufferUsage::OneTimeSubmit && metalCommandBuffer->bufferHasBeenUsed()){
                        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "ComputeCommandBuffer recorded with CommandBufferUsage::OneTimeSubmit has already been used. Only buffers recorded with CommandBufferUsage::Default can submitted multiples times after being recorded once.");
                        break;
                    }
                    
                    id<MTLCommandBuffer> executionBuffer{ [commandQueue commandBuffer] };
                    id<MTLComputeCommandEncoder> encoder{ [executionBuffer computeCommandEncoder] };
                    
                    //Inject the wait semaphore into each buffer
                    for (auto const &semaphore : submission.waitSemaphores) {
                        auto *metalSemaphore{ polyCast<MetalSemaphore const>(semaphore.first.get()) };
                        
                        [encoder waitForFence:metalSemaphore->getFence()];
                    }
                    
                    //Excute all recorded commands for the encoder
                    for(auto &command : metalCommandBuffer->getCommands()) {
                        command(encoder);
                    }
                    
                    //For the last buffer add all semaphore signalling
                    if(isLastCommandBuffer && !submission.signalSemaphores.empty()) {
                        for(auto const &semaphore : submission.signalSemaphores) {
                            [encoder updateFence:polyCast<MetalSemaphore const>(semaphore.get())->getFence()];
                        }
                    }
                    
                    //For the last submission signal the fence
                    if(isLastSubmission && signalFence != nullptr) {
                        __block auto *fence{ polyCast<MetalFence>(signalFence) };
                        [executionBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
                            fence->signal();
                        }];
                    }
                    
                    [encoder endEncoding];
                    [executionBuffer commit];
                    
                    metalCommandBuffer->markAsUsed();
                }
            }
        }
    }
}
