#pragma once

#include "Clove/Graphics/GhaDescriptorPool.hpp"

namespace clove {
    template<typename BasePoolType>
    class ValidationDescriptorPool : public BasePoolType {
        //FUNCTIONS
    public:
        using BasePoolType::BasePoolType;

        void freeDescriptorSets(std::unique_ptr<GhaDescriptorSet> &descriptorSet) override;
        void freeDescriptorSets(std::vector<std::unique_ptr<GhaDescriptorSet>> &descriptorSets) override;
    };
}

#include "ValidationDescriptorPool.inl"