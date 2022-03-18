#include "Clove/Graphics/Metal/MetalGraphicsCommandBuffer.hpp"

#include "Clove/Graphics/Metal/MetalFramebuffer.hpp"
#include "Clove/Graphics/Metal/MetalGlobals.hpp"
#include "Clove/Graphics/Metal/MetalGraphicsPipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalBuffer.hpp"
#include "Clove/Graphics/Metal/MetalImage.hpp"
#include "Clove/Graphics/Metal/MetalRenderPass.hpp"
#include "Clove/Graphics/Metal/MetalDescriptorSet.hpp"
#include "Clove/Graphics/Metal/MetalDescriptorSetLayout.hpp"
#include "Clove/Graphics/Metal/MetalPipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    namespace {
        MTLIndexType getIndexType(IndexType garlicType) {
            switch(garlicType) {
                case IndexType::Uint16:
                    return MTLIndexTypeUInt16;
                default:
                    CLOVE_ASSERT_MSG(false, "{0}: Unkown index type", CLOVE_FUNCTION_NAME);
                    return MTLIndexTypeUInt16;
            }
        }
    }
    
    MetalGraphicsCommandBuffer::MetalGraphicsCommandBuffer(id<MTLCommandBuffer> commandBuffer)
    : commandBuffer{ commandBuffer } {
    }
    
    MetalGraphicsCommandBuffer::MetalGraphicsCommandBuffer(MetalGraphicsCommandBuffer &&other) noexcept = default;
    
    MetalGraphicsCommandBuffer& MetalGraphicsCommandBuffer::operator=(MetalGraphicsCommandBuffer &&other) noexcept = default;
    
    MetalGraphicsCommandBuffer::~MetalGraphicsCommandBuffer() = default;
    
    void MetalGraphicsCommandBuffer::beginRecording() {
        //no op
    }
    
    void MetalGraphicsCommandBuffer::endRecording() {
        //no op
    }
    
    void MetalGraphicsCommandBuffer::beginRenderPass(GhaRenderPass &renderPass, GhaFramebuffer &frameBuffer, RenderArea const &renderArea, std::span<ClearValue> clearValues) {
        GhaRenderPass::Descriptor const &renderPassDescriptor{ polyCast<MetalRenderPass>(&renderPass)->getDescriptor() };
        
        //Modify the attachments to have the correct clear values.
        MTLRenderPassDescriptor *frameBufferDescriptor{ polyCast<MetalFramebuffer>(&frameBuffer)->getRenderPassDescriptor() };
        for(size_t i{ 0 }; i < renderPassDescriptor.colourAttachments.size(); ++i) {
            ColourValue const colour{ std::get<ColourValue>(clearValues[i]) };
            frameBufferDescriptor.colorAttachments[i].clearColor = MTLClearColor{ colour.r, colour.g, colour.b, colour.a };
        }
        DepthStencilValue const depthStencilValue{ std::get<DepthStencilValue>(clearValues.back()) };
        frameBufferDescriptor.depthAttachment.clearDepth = depthStencilValue.depth;
        
        //TODO: RenderArea
        
        currentPass = [commandBuffer renderCommandEncoderWithDescriptor:frameBufferDescriptor];
        passes.push_back(currentPass);
    }
    
    void MetalGraphicsCommandBuffer::endRenderPass() {
        currentPass = nullptr;
    }
    
    void MetalGraphicsCommandBuffer::setViewport(vec2i position, vec2ui size) {
        MTLViewport const viewport {
            .originX = static_cast<double>(position.x),
            .originY = static_cast<double>(position.y),
            .width   = static_cast<double>(size.x),
            .height  = static_cast<double>(size.y),
            .znear   = 0.0f,
            .zfar    = 1.0f,
        };
        
        [currentPass setViewport:viewport];
    }
    
    void MetalGraphicsCommandBuffer::setScissor(vec2i position, vec2ui size) {
        MTLScissorRect const scissorRect {
            .x      = static_cast<NSUInteger>(position.x),
            .y      = static_cast<NSUInteger>(position.y),
            .width  = size.x,
            .height = size.y,
        };
        
        [currentPass setScissorRect:scissorRect];
    }
    
    void MetalGraphicsCommandBuffer::bindPipelineObject(GhaGraphicsPipelineObject &pipelineObject) {
        auto const *const metalPipeline{ polyCast<MetalGraphicsPipelineObject const>(&pipelineObject) };
        if(metalPipeline == nullptr){
            CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: PipelineObject is nullptr", CLOVE_FUNCTION_NAME);
            return;
        }
        
        MTLWinding constexpr winding{ MTLWindingClockwise };
        
        [currentPass setRenderPipelineState:metalPipeline->getPipeline()];
        [currentPass setDepthStencilState:metalPipeline->getDepthStencil()];
        [currentPass setFrontFacingWinding:winding];
    }
    
    void MetalGraphicsCommandBuffer::bindVertexBuffer(GhaBuffer &vertexBuffer, size_t const offset) {
        [currentPass setVertexBuffer:polyCast<MetalBuffer>(&vertexBuffer)->getBuffer()
                              offset:offset
                             atIndex:vertexBufferBindingIndex];
    }
    
    void MetalGraphicsCommandBuffer::bindIndexBuffer(GhaBuffer &indexBuffer, size_t const offset, IndexType indexType) {
        cachedIndexBuffer.buffer    = polyCast<MetalBuffer>(&indexBuffer)->getBuffer();
        cachedIndexBuffer.indexType = getIndexType(indexType);
        cachedIndexBuffer.offset    = offset;
    }
    
    void MetalGraphicsCommandBuffer::bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) {
        auto const *const metalDescriptorSet{ polyCast<MetalDescriptorSet>(&descriptorSet) };
        if(metalDescriptorSet == nullptr) {
            CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: DescriptorSet is nullptr", CLOVE_FUNCTION_NAME);
            return;
        }
        
        id<MTLBuffer> backingBuffer{ metalDescriptorSet->getBackingBuffer() };
        
        std::optional<size_t> vertexOffset{ metalDescriptorSet->getVertexOffset() };
        std::optional<size_t> pixelOffset{ metalDescriptorSet->getPixelOffset() };
        
        if(vertexOffset.has_value()) {
            [currentPass setVertexBuffer:backingBuffer
                                  offset:vertexOffset.value()
                                 atIndex:setNum];
        }
        if(pixelOffset.has_value()) {
            [currentPass setFragmentBuffer:backingBuffer
                                    offset:pixelOffset.value()
                                   atIndex:setNum];
        }
    }
    
    void MetalGraphicsCommandBuffer::pushConstant(GhaShader::Stage const stage, size_t const offset, size_t const size, void const *data) {
        switch(stage) {
            case GhaShader::Stage::Vertex:
                [currentPass setVertexBytes:data
                                     length:size
                                    atIndex:pushConstantSlot];
                break;
            case GhaShader::Stage::Pixel:
                [currentPass setFragmentBytes:data
                                       length:size
                                      atIndex:pushConstantSlot];
                break;
            default:
                CLOVE_ASSERT_MSG(false, "{0}: Unknown shader stage provided", CLOVE_FUNCTION_NAME_PRETTY);
                break;
        }
    }
    
    void MetalGraphicsCommandBuffer::drawIndexed(size_t const indexCount) {
        [currentPass drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                indexCount:indexCount
                                 indexType:cachedIndexBuffer.indexType
                               indexBuffer:cachedIndexBuffer.buffer
                         indexBufferOffset:cachedIndexBuffer.offset];
    }
    
    void MetalGraphicsCommandBuffer::bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
        if(currentPass != nullptr){
            id<MTLBuffer> mtlBuffer{ polyCast<MetalBuffer>(&buffer)->getBuffer() };
            
            [currentPass memoryBarrierWithResources:&mtlBuffer
                                              count:1
                                        afterStages:convertStage(sourceStage)
                                       beforeStages:convertStage(destinationStage)];
        }
    }
    
    void MetalGraphicsCommandBuffer::imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
        if(currentPass != nullptr){
            id<MTLTexture> mtlTexture{ polyCast<MetalImage>(&image)->getTexture() };
            
            [currentPass memoryBarrierWithResources:&mtlTexture
                                              count:1
                                        afterStages:convertStage(sourceStage)
                                       beforeStages:convertStage(destinationStage)];
        }
    }
}
