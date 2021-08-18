#include "Clove/Graphics/Metal/MetalDescriptorSet.hpp"

#include "Clove/Graphics/GhaDescriptorSetLayout.hpp"
#include "Clove/Graphics/Metal/MetalBuffer.hpp"
#include "Clove/Graphics/Metal/MetalImageView.hpp"
#include "Clove/Graphics/Metal/MetalSampler.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalDescriptorSet::MetalDescriptorSet(id<MTLArgumentEncoder> vertexEncoder, id<MTLBuffer> vertexEncoderBuffer, id<MTLArgumentEncoder> pixelEncoder, id<MTLBuffer> pixelEncoderBuffer, id<MTLArgumentEncoder> computeEncoder, id<MTLBuffer> computeEncoderBuffer, std::shared_ptr<GhaDescriptorSetLayout> layout)
        : vertexEncoder{ vertexEncoder }
        , vertexEncoderBuffer{ vertexEncoderBuffer }
        , pixelEncoder{ pixelEncoder }
        , pixelEncoderBuffer{ pixelEncoderBuffer }
        , computeEncoder{ computeEncoder }
        , computeEncoderBuffer{ computeEncoderBuffer }
        , layout{ std::move(layout) } {
    }
    
    MetalDescriptorSet::MetalDescriptorSet(MetalDescriptorSet &&other) noexcept = default;
    
    MetalDescriptorSet &MetalDescriptorSet::operator=(MetalDescriptorSet &&other) noexcept = default;
    
    MetalDescriptorSet::~MetalDescriptorSet() = default;
    
    void MetalDescriptorSet::map(GhaBuffer const &buffer, size_t const offset, size_t const range, DescriptorType const descriptorType, uint32_t const bindingSlot) {
        GhaShader::Stage const shaderStage{ getStageFromBindingSlot(bindingSlot) };
        id<MTLBuffer> mtlBuffer{ polyCast<MetalBuffer const>(&buffer)->getBuffer() };
        
        if((shaderStage & GhaShader::Stage::Vertex) != 0){
            [vertexEncoder setBuffer:mtlBuffer
                              offset:offset
                             atIndex:bindingSlot];
        }
        if((shaderStage & GhaShader::Stage::Pixel) != 0){
            [pixelEncoder setBuffer:mtlBuffer
                             offset:offset
                            atIndex:bindingSlot];
        }
        if((shaderStage & GhaShader::Stage::Compute) != 0){
            [computeEncoder setBuffer:mtlBuffer
                               offset:offset
                              atIndex:bindingSlot];
        }
    }
    
    void MetalDescriptorSet::map(GhaImageView const &imageView, GhaImage::Layout const layout, uint32_t const bindingSlot) {
        [pixelEncoder setTexture:polyCast<MetalImageView const>(&imageView)->getTexture()
                         atIndex:bindingSlot];
    }
    
    void MetalDescriptorSet::map(GhaSampler const &sampler, uint32_t const bindingSlot) {
        [pixelEncoder setSamplerState:polyCast<MetalSampler const>(&sampler)->getSamplerState()
                              atIndex:bindingSlot];
    }
    
    GhaShader::Stage MetalDescriptorSet::getStageFromBindingSlot(uint32_t const bindingSlot) {
        for(auto const &binding : layout->getDescriptor().bindings) {
            if(binding.binding == bindingSlot) {
                return binding.stage;
            }
        }
        
        CLOVE_ASSERT(false, "{0}: Could not find binding", CLOVE_FUNCTION_NAME_PRETTY);
        return GhaShader::Stage::Vertex;
    }
}
