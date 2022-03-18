#include "Clove/Graphics/Metal/MetalComputeCommandBuffer.hpp"

#include "Clove/Graphics/Metal/MetalBuffer.hpp"
#include "Clove/Graphics/Metal/MetalComputePipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalDescriptorSet.hpp"
#include "Clove/Graphics/Metal/MetalGlobals.hpp"
#include "Clove/Graphics/Metal/MetalImage.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"
#include "Clove/Graphics/Metal/MetalShader.hpp"

#include <Clove/Cast.hpp>

namespace clove {
	MetalComputeCommandBuffer::MetalComputeCommandBuffer(id<MTLCommandBuffer> commandBuffer) 
		: commandBuffer{ commandBuffer } {
	}
	
	MetalComputeCommandBuffer::MetalComputeCommandBuffer(MetalComputeCommandBuffer &&other) noexcept = default;
	
	MetalComputeCommandBuffer &MetalComputeCommandBuffer::operator=(MetalComputeCommandBuffer &&other) noexcept = default;
	
	MetalComputeCommandBuffer::~MetalComputeCommandBuffer() = default;

	void MetalComputeCommandBuffer::beginRecording() {
        encoder = [commandBuffer computeCommandEncoder];
	}
	
	void MetalComputeCommandBuffer::endRecording() {
		//no op
	}

	void MetalComputeCommandBuffer::bindPipelineObject(GhaComputePipelineObject &pipelineObject) {
        auto *const metalPipelineObject{ polyCast<MetalComputePipelineObject>(&pipelineObject) };
        activePipeline = metalPipelineObject;
        
		[encoder setComputePipelineState:metalPipelineObject->getPipelineState()];
	}

	void MetalComputeCommandBuffer::bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) {
		auto const *const metalDescriptorSet{ polyCast<MetalDescriptorSet>(&descriptorSet) };
        if(metalDescriptorSet == nullptr) {
            CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: DescriptorSet is nullptr", CLOVE_FUNCTION_NAME);
            return;
        }
        
        id<MTLBuffer> backingBuffer{ metalDescriptorSet->getBackingBuffer() };
        std::optional<size_t> computeOffset{ metalDescriptorSet->getComputeOffset() };
        CLOVE_ASSERT(computeOffset.has_value());
        
        [encoder setBuffer:backingBuffer
                    offset:computeOffset.value()
                   atIndex:setNum];
	}
	
	void MetalComputeCommandBuffer::pushConstant(size_t const offset, size_t const size, void const *data) {
		[encoder setBytes:data
				   length:size
				  atIndex:pushConstantSlot];
	}

	void MetalComputeCommandBuffer::disptach(vec3ui groupCount) {
        auto const *const metalShader{ polyCast<MetalShader const>(activePipeline->getDescriptor().shader) };
        vec3ui const workGroupSize{ metalShader != nullptr ? metalShader->getWorkgroupSize() : vec3ui{ 1, 1, 1 } };
        
		[encoder dispatchThreadgroups:MTLSizeMake(groupCount.x, groupCount.y, groupCount.z)
				threadsPerThreadgroup:MTLSizeMake(workGroupSize.x, workGroupSize.y, workGroupSize.z)];
	}

	void MetalComputeCommandBuffer::bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
		id<MTLBuffer> mtlBuffer{ polyCast<MetalBuffer>(&buffer)->getBuffer() };

		[encoder memoryBarrierWithResources:&mtlBuffer
									  count:1];
	}
	
	void MetalComputeCommandBuffer::imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
		id<MTLTexture> mtlTexture{ polyCast<MetalImage>(&image)->getTexture() };

		[encoder memoryBarrierWithResources:&mtlTexture
									  count:1];
	}
}
