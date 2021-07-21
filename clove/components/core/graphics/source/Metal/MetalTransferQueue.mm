#include "Clove/Graphics/Metal/MetalTransferQueue.hpp"

#include "Clove/Graphics/Metal/MetalPipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalTransferCommandBuffer.hpp"
#include "Clove/Graphics/Metal/MetalSemaphore.hpp"
#include "Clove/Graphics/Metal/MetalFence.hpp"

#include <Clove/Cast.hpp>

namespace garlic::clove {
	MetalTransferQueue::MetalTransferQueue(CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue)
		: commandQueue{ [commandQueue retain] } {
		allowBufferReuse = (descriptor.flags & QueueFlags::ReuseBuffers) != 0;
	}
	
	MetalTransferQueue::MetalTransferQueue(MetalTransferQueue &&other) noexcept = default;
	
	MetalTransferQueue& MetalTransferQueue::operator=(MetalTransferQueue &&other) noexcept = default;
	
	MetalTransferQueue::~MetalTransferQueue() {
		[commandQueue release];
	}

	std::unique_ptr<GhaTransferCommandBuffer> MetalTransferQueue::allocateCommandBuffer() {
		return std::make_unique<MetalTransferCommandBuffer>(allowBufferReuse);
	}
	
	void MetalTransferQueue::freeCommandBuffer(GhaTransferCommandBuffer &buffer) {
		//no op
	}

	void MetalTransferQueue::submit(std::vector<TransferSubmitInfo> const &submissions, GhaFence *signalFence) {
		@autoreleasepool {
			for(size_t i{ 0 }; i < submissions.size(); ++i) {
				auto const &submission{ submissions[i] };
				bool const isLastSubmission{ i == submissions.size() - 1 };
				
				for(auto const &commandBuffer : submission.commandBuffers) {
					bool const isLastCommandBuffer{ commandBuffer == submission.commandBuffers.back() };
					
					auto *metalCommandBuffer{ polyCast<MetalTransferCommandBuffer>(commandBuffer.get()) };
					if(metalCommandBuffer->getCommandBufferUsage() == CommandBufferUsage::OneTimeSubmit && metalCommandBuffer->bufferHasBeenUsed()){
						CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "TransferCommandBuffer recorded with CommandBufferUsage::OneTimeSubmit has already been used. Only buffers recorded with CommandBufferUsage::Default can submitted multiples times after being recorded once.");
						break;
					}
					
					id<MTLCommandBuffer> executionBuffer{ [commandQueue commandBuffer] };
					id<MTLBlitCommandEncoder> encoder{ [executionBuffer blitCommandEncoder] };
					
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