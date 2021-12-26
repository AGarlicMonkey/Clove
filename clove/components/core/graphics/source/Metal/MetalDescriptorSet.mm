#include "Clove/Graphics/Metal/MetalDescriptorSet.hpp"

#include "Clove/Graphics/GhaDescriptorSetLayout.hpp"
#include "Clove/Graphics/Metal/MetalBuffer.hpp"
#include "Clove/Graphics/Metal/MetalImageView.hpp"
#include "Clove/Graphics/Metal/MetalSampler.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    MetalDescriptorSet::MetalDescriptorSet(std::optional<ArgumentEncoder> vertexEncoder, std::optional<ArgumentEncoder> pixelEncoder, std::optional<ArgumentEncoder> computeEncoder, id<MTLBuffer> backingBuffer, GhaDescriptorSetLayout const *layout)
        : vertexEncoder{ std::move(vertexEncoder) }
        , pixelEncoder{ std::move(pixelEncoder) }
        , computeEncoder{ std::move(computeEncoder) }
        , backingBuffer{ backingBuffer }
        , layout{ layout } {
    }
    
    MetalDescriptorSet::MetalDescriptorSet(MetalDescriptorSet &&other) noexcept = default;
    
    MetalDescriptorSet &MetalDescriptorSet::operator=(MetalDescriptorSet &&other) noexcept = default;
    
    MetalDescriptorSet::~MetalDescriptorSet() = default;
    
    void MetalDescriptorSet::write(GhaBuffer const &buffer, size_t const offset, size_t const range, DescriptorType const descriptorType, uint32_t const bindingSlot) {
        GhaShader::Stage const shaderStage{ getStageFromBindingSlot(bindingSlot) };
        id<MTLBuffer> mtlBuffer{ polyCast<MetalBuffer const>(&buffer)->getBuffer() };
        
        if((shaderStage & GhaShader::Stage::Vertex) != 0) {
            [vertexEncoder->encoder setBuffer:mtlBuffer
                                       offset:offset
                                      atIndex:bindingSlot];
        }
        if((shaderStage & GhaShader::Stage::Pixel) != 0) {
            [pixelEncoder->encoder setBuffer:mtlBuffer
                                      offset:offset
                                     atIndex:bindingSlot];
        }
        if((shaderStage & GhaShader::Stage::Compute) != 0) {
            [computeEncoder->encoder setBuffer:mtlBuffer
                                        offset:offset
                                       atIndex:bindingSlot];
        }
    }
    
    void MetalDescriptorSet::write(GhaImageView const &imageView, GhaImage::Layout const layout, DescriptorType const descriptorType, uint32_t const bindingSlot) {
        [pixelEncoder->encoder setTexture:polyCast<MetalImageView const>(&imageView)->getTexture()
                                  atIndex:bindingSlot];
    }
    
    void MetalDescriptorSet::write(GhaSampler const &sampler, uint32_t const bindingSlot) {
        [pixelEncoder->encoder setSamplerState:polyCast<MetalSampler const>(&sampler)->getSamplerState()
                                       atIndex:bindingSlot];
    }
    
    GhaShader::Stage MetalDescriptorSet::getStageFromBindingSlot(uint32_t const bindingSlot) {
        for(auto const &binding : layout->getDescriptor().bindings) {
            if(binding.binding == bindingSlot) {
                return binding.stage;
            }
        }
        
        CLOVE_ASSERT_MSG(false, "{0}: Could not find binding", CLOVE_FUNCTION_NAME_PRETTY);
        return GhaShader::Stage::Vertex;
    }
}
