#include "Clove/Graphics/Metal/MetalShader.hpp"

namespace clove {
    MetalShader::MetalShader(id<MTLFunction> function)
        : function{ function }{
    }
    
    MetalShader::MetalShader(MetalShader &&other) noexcept = default;
    
    MetalShader& MetalShader::operator=(MetalShader &&other) noexcept = default;
    
    MetalShader::~MetalShader() = default;
}
