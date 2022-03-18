#pragma once

#include "Clove/Graphics/GhaGraphicsCommandBuffer.hpp"

#include <MetalKit/MetalKit.h>
#include <vector>
#include <functional>

namespace clove {
	class MetalGraphicsCommandBuffer : public GhaGraphicsCommandBuffer {
	private:
		struct CachedIndexBufferData {
			id<MTLBuffer> buffer{};
			MTLIndexType indexType{};
			NSUInteger offset{};
		};
		
		//VARIABLES
	private:
        id<MTLCommandBuffer> commandBuffer{ nullptr };
        id<MTLRenderCommandEncoder> currentPass{ nullptr };

        std::vector<id<MTLRenderCommandEncoder>> passes{};

        //Metal's drawIndexed call takes an index buffer directly so we need to cache the one provided from bindIndexBuffer
		CachedIndexBufferData cachedIndexBuffer;
		
		//FUNCTIONS
	public:
        MetalGraphicsCommandBuffer() = delete;
        MetalGraphicsCommandBuffer(id<MTLCommandBuffer> commandBuffer);

        MetalGraphicsCommandBuffer(MetalGraphicsCommandBuffer const &other) = delete;
		MetalGraphicsCommandBuffer(MetalGraphicsCommandBuffer &&other) noexcept;
		
		MetalGraphicsCommandBuffer& operator=(MetalGraphicsCommandBuffer const &other) = delete;
		MetalGraphicsCommandBuffer& operator=(MetalGraphicsCommandBuffer &&other) noexcept;
		
		~MetalGraphicsCommandBuffer();
		
		void beginRecording() override;
		void endRecording() override;

		void beginRenderPass(GhaRenderPass &renderPass, GhaFramebuffer &frameBuffer, RenderArea const &renderArea, std::span<ClearValue> clearValues) override;
		void endRenderPass() override;

		void setViewport(vec2i position, vec2ui size) override;
		void setScissor(vec2i position, vec2ui size) override;

		void bindPipelineObject(GhaGraphicsPipelineObject &pipelineObject) override;
		void bindVertexBuffer(GhaBuffer &vertexBuffer, size_t const offset) override;
		void bindIndexBuffer(GhaBuffer &indexBuffer, size_t const offset, IndexType indexType) override;
		
		void bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) override;
		void pushConstant(GhaShader::Stage const stage, size_t const offset, size_t const size, void const *data) override;

		void drawIndexed(size_t const indexCount) override;

		void bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;
		void imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) override;

        inline id<MTLCommandBuffer> getMtlCommandBuffer() const;
        inline std::vector<id<MTLRenderCommandEncoder>> const &getRenderPasses() const;
    };
}

#include "Clove/Graphics/Metal/MetalGraphicsCommandBuffer.inl"