#pragma once

#include "Clove/Graphics/GhaGraphicsQueue.hpp"

#include <MetalKit/MetalKit.h>

@class MetalView;

namespace garlic::clove {
	class MetalGraphicsQueue : public GhaGraphicsQueue {
		//VARIABLES
	private:
		id<MTLCommandQueue> commandQueue;

		//FUNCTIONS
	public:
		MetalGraphicsQueue() = delete;
		MetalGraphicsQueue(id<MTLCommandQueue> commandQueue);
		
		MetalGraphicsQueue(MetalGraphicsQueue const &other) = delete;
		MetalGraphicsQueue(MetalGraphicsQueue &&other) noexcept;
		
		MetalGraphicsQueue& operator=(MetalGraphicsQueue const &other) = delete;
		MetalGraphicsQueue& operator=(MetalGraphicsQueue &&other) noexcept;
		
		~MetalGraphicsQueue();

		std::unique_ptr<GhaGraphicsCommandBuffer> allocateCommandBuffer() override;
		void freeCommandBuffer(GhaGraphicsCommandBuffer &buffer) override;

		void submit(std::vector<GraphicsSubmitInfo> const &submissions, GhaFence const *signalFence) override;
	};
}
