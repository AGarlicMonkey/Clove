#pragma once

#include "Clove/Graphics/GhaTransferCommandBuffer.hpp"

#include <MetalKit/MetalKit.h>
#include <vector>
#include <functional>

namespace clove {
	class MetalTransferCommandBuffer : public GhaTransferCommandBuffer {
		//VARIABLES
	private:
		std::vector<std::function<void(id<MTLBlitCommandEncoder>)>> commands{};
			
		//FUNCTIONS
	public:
        MetalTransferCommandBuffer();
		
		MetalTransferCommandBuffer(MetalTransferCommandBuffer const &other) = delete;
		MetalTransferCommandBuffer(MetalTransferCommandBuffer &&other) noexcept;
		
		MetalTransferCommandBuffer &operator=(MetalTransferCommandBuffer const &other) = delete;
		MetalTransferCommandBuffer &operator=(MetalTransferCommandBuffer &&other) noexcept;
		
		~MetalTransferCommandBuffer();

		void beginRecording(CommandBufferUsage usageFlag) override;
		void endRecording() override;

		void copyBufferToBuffer(GhaBuffer &source, size_t const sourceOffset, GhaBuffer &destination, size_t const destinationOffset, size_t const sizeBytes) override;
        void copyBufferToImage(GhaBuffer &source, size_t const sourceOffset, GhaImage &destination, vec3i const &destinationOffset, vec3ui const &destinationExtent, uint32_t const destinationBaseLayer, uint32_t const destinationLayerCount) override;

        void copyImageToBuffer(GhaImage &source, vec3i const &sourceOffset, vec3ui const &sourceExtent, uint32_t const sourceBaseLayer, uint32_t const sourceLayerCount, GhaBuffer &destination, size_t const destinationOffset) override;

        void bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;
		void imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;
		
		inline std::vector<std::function<void(id<MTLBlitCommandEncoder>)>> const &getCommands() const;
	};
}

#include "MetalTransferCommandBuffer.inl"
