namespace clove {
    GhaDescriptorSetLayout::Descriptor const &VulkanDescriptorSetLayout::getDescriptor() const {
        return descriptor;
    }

    VkDescriptorSetLayout VulkanDescriptorSetLayout::getLayout() const {
        return layout;
    }
}