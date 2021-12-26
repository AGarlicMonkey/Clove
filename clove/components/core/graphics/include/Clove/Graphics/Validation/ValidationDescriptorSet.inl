#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename BaseDescriptorSetType>
    void ValidationDescriptorSet<BaseDescriptorSetType>::write(GhaBuffer const &buffer, size_t const offset, size_t const range, DescriptorType const descriptorType, uint32_t const bindingSlot) {
        CLOVE_ASSERT_MSG(range > 0, "{0}: 'range' value cannot be 0.", CLOVE_FUNCTION_NAME);

        CLOVE_ASSERT_MSG(descriptorType == DescriptorType::UniformBuffer || descriptorType == DescriptorType::StorageBuffer, "{0}: Invalid descriptor type for buffer.", CLOVE_FUNCTION_NAME);

        BaseDescriptorSetType::write(buffer, offset, range, descriptorType, bindingSlot);
    }

    template<typename BaseDescriptorSetType>
    void ValidationDescriptorSet<BaseDescriptorSetType>::write(GhaImageView const &imageView, GhaImage::Layout const layout, DescriptorType const descriptorType, uint32_t const bindingSlot) {
        CLOVE_ASSERT_MSG(descriptorType == DescriptorType::SampledImage || descriptorType == DescriptorType::StorageImage, "{0}: Invalid descriptor type for image.", CLOVE_FUNCTION_NAME);

        BaseDescriptorSetType::write(imageView, layout, descriptorType, bindingSlot);
    }
}