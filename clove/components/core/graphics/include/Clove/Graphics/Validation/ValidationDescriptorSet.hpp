#pragma once

namespace clove {
    class GhaBuffer;
    class GhaImageView;
}

namespace clove {
    template<typename BaseDescriptorSetType>
    class ValidationDescriptorSet : public BaseDescriptorSetType {
        //FUNCTIONS
    public:
        using BaseDescriptorSetType::BaseDescriptorSetType;

        void write(GhaBuffer const &buffer, size_t const offset, size_t const range, DescriptorType const descriptorType, uint32_t const bindingSlot) override;
        void write(GhaImageView const &imageView, GhaImage::Layout const layout, DescriptorType const descriptorType, uint32_t const bindingSlot) override;
    };
}

#include "ValidationDescriptorSet.inl"