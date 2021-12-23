#include "Clove/Graphics/Metal/MetalShader.hpp"

namespace clove {
    MetalShader::MetalShader(id<MTLFunction> function, vec3ui workgroupSize)
        : function{ function }
        , workgroupSize{ workgroupSize }{
    }
    
    MetalShader::MetalShader(MetalShader &&other) noexcept = default;
    
    MetalShader& MetalShader::operator=(MetalShader &&other) noexcept = default;
    
    MetalShader::~MetalShader() = default;
}
