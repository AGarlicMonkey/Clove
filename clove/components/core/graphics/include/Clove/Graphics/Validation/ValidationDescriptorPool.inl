#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename BasePoolType>
    void ValidationDescriptorPool<BasePoolType>::freeDescriptorSets(std::unique_ptr<GhaDescriptorSet> &descriptorSet) {
        CLOVE_ASSERT_MSG(BasePoolType::getDescriptor().flag == GhaDescriptorPool::Flag::FreeDescriptorSet, "{0}: Cannot manually free descriptor sets from a pool created without GhaDescriptorPool::Flag::FreeDescriptorSet", CLOVE_FUNCTION_NAME_PRETTY);

        BasePoolType::freeDescriptorSets(descriptorSet);
    }

    template<typename BasePoolType>
    void ValidationDescriptorPool<BasePoolType>::freeDescriptorSets(std::vector<std::unique_ptr<GhaDescriptorSet>> &descriptorSets) {
        CLOVE_ASSERT_MSG(BasePoolType::getDescriptor().flag == GhaDescriptorPool::Flag::FreeDescriptorSet, "{0}: Cannot manually free descriptor sets from a pool created without GhaDescriptorPool::Flag::FreeDescriptorSet", CLOVE_FUNCTION_NAME_PRETTY);

        BasePoolType::freeDescriptorSets(descriptorSets);
    }
}