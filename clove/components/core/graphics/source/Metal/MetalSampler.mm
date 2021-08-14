#include "Clove/Graphics/Metal/MetalSampler.hpp"

namespace garlic::clove {
    MetalSampler::MetalSampler(id<MTLSamplerState> samplerState)
        : samplerState{ samplerState } {
    }
    
    MetalSampler::MetalSampler(MetalSampler &&other) noexcept = default;
    
    MetalSampler& MetalSampler::operator=(MetalSampler &&other) noexcept = default;
    
    MetalSampler::~MetalSampler() = default;
}
