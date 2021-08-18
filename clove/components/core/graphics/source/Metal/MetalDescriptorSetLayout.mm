#include "Clove/Graphics/Metal/MetalDescriptorSetLayout.hpp"

namespace clove {
    MetalDescriptorSetLayout::MetalDescriptorSetLayout(Descriptor descriptor, NSArray<MTLArgumentDescriptor *> *vertexDescriptors, NSArray<MTLArgumentDescriptor *> *pixelDescriptors, NSArray<MTLArgumentDescriptor *> *computeDescriptors)
        : descriptor{ std::move(descriptor) }
        , vertexDescriptors{ vertexDescriptors }
        , pixelDescriptors{ pixelDescriptors }
        , computeDescriptors{ computeDescriptors} {
    }
    
    MetalDescriptorSetLayout::MetalDescriptorSetLayout(MetalDescriptorSetLayout &&other) noexcept = default;
    
    MetalDescriptorSetLayout& MetalDescriptorSetLayout::operator=(MetalDescriptorSetLayout &&other) noexcept = default;
    
    MetalDescriptorSetLayout::~MetalDescriptorSetLayout() = default;
    
    GhaDescriptorSetLayout::Descriptor const &MetalDescriptorSetLayout::getDescriptor() const {
        return descriptor;
    }
}
