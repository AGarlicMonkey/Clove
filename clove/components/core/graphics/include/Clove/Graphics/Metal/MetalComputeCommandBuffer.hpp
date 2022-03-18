#pragma once

#include "Clove/Graphics/GhaComputeCommandBuffer.hpp"

#include <MetalKit/MetalKit.h>
#include <vector>
#include <functional>

namespace clove {
    class MetalComputePipelineObject;
}

namespace clove {
	class MetalComputeCommandBuffer : public GhaComputeCommandBuffer {
		//VARIABLES
	private:
        id<MTLCommandBuffer> commandBuffer{ nullptr };
        id<MTLComputeCommandEncoder> encoder{ nullptr };

        MetalComputePipelineObject *activePipeline{ nullptr }; //Required to get shader workgroup size
			
		//FUNCTIONS
	public:
        MetalComputeCommandBuffer() = delete;
        MetalComputeCommandBuffer(id<MTLCommandBuffer> commandBuffer);

        MetalComputeCommandBuffer(MetalComputeCommandBuffer const &other) = delete;
		MetalComputeCommandBuffer(MetalComputeCommandBuffer &&other) noexcept;
		
		MetalComputeCommandBuffer &operator=(MetalComputeCommandBuffer const &other) = delete;
		MetalComputeCommandBuffer &operator=(MetalComputeCommandBuffer &&other) noexcept;
		
		~MetalComputeCommandBuffer();

		void beginRecording() override;
        void endRecording() override;

		void bindPipelineObject(GhaComputePipelineObject &pipelineObject) override;

		void bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) override;
		void pushConstant(size_t const offset, size_t const size, void const *data) override;

		void disptach(vec3ui groupCount) override;

		void bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;
		void imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;

        inline id<MTLCommandBuffer> getMtlCommandBuffer() const;
        inline id<MTLComputeCommandEncoder> getEncoder() const;
    };
}

#include "MetalComputeCommandBuffer.inl"