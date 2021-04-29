#include "Clove/Graphics/Metal/MetalGraphicsQueue.hpp"

#include "Clove/Graphics/Metal/MetalGraphicsCommandBuffer.hpp"
#include "Clove/Graphics/Metal/MetalView.hpp"

#include <Clove/Cast.hpp>

namespace garlic::clove {
	MetalGraphicsQueue::MetalGraphicsQueue(id<MTLCommandQueue> commandQueue)
		: commandQueue{ commandQueue } {
	}
	
	MetalGraphicsQueue::MetalGraphicsQueue(MetalGraphicsQueue &&other) noexcept = default;
	
	MetalGraphicsQueue& MetalGraphicsQueue::operator=(MetalGraphicsQueue &&other) noexcept = default;
	
	MetalGraphicsQueue::~MetalGraphicsQueue() {
		[commandQueue release];
	}
	
	std::unique_ptr<GhaGraphicsCommandBuffer> MetalGraphicsQueue::allocateCommandBuffer() {
		return std::make_unique<MetalGraphicsCommandBuffer>([commandQueue commandBuffer]);
	}
	
	void MetalGraphicsQueue::freeCommandBuffer(GhaGraphicsCommandBuffer &buffer) {
		//no op
	}

	void MetalGraphicsQueue::submit(std::vector<GraphicsSubmitInfo> const &submissions, GhaFence const *signalFence) {
		for(auto &submission : submissions) {
			//TODO: Fences / semahpores
			//[currentEncoder updateFence:nullptr afterStages:MTLRenderStageFragment];
			
			for(auto &commandBuffer : submission.commandBuffers) {
				id<MTLCommandBuffer> mtlCommandBuffer{ polyCast<MetalGraphicsCommandBuffer>(commandBuffer.get())->getCommandBuffer() };
				[mtlCommandBuffer commit];
			}
		}
	}
}