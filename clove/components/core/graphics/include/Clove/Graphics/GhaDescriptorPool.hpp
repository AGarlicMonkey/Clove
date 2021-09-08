#pragma once

#include "Clove/Graphics/Descriptor.hpp"

#include <cinttypes>
#include <memory>
#include <vector>

namespace clove {
    class GhaDescriptorSet;
    class GhaDescriptorSetLayout;

    /**
     * @brief Information on each type of descriptor allocated from a pool.
     */
    struct DescriptorInfo {
        DescriptorType type;
        uint32_t count; /**< Number of this type to allocate across all DescriptorSets. For example: DescriptorSetBindingInfo::arraySize * GhaDescriptorPool::Descriptor::maxSets. */
    };
}

namespace clove {
    /**
     * @brief Used to allocate GhaDescriptorSets. GhaDescriptorPool is not tied to a specific GhaDescriptorSetLayout.
     */
    class GhaDescriptorPool {
        //TYPES
    public:
        enum class Flag {
            None,
            FreeDescriptorSet /**< DescriptorSets can be freed individualy. Some platforms might use a more simple allocator if this isn't set. */
        };

        struct Descriptor {
            std::vector<DescriptorInfo> poolTypes{};
            Flag flag{ Flag::None };
            uint32_t maxSets{ 0 }; /**< The maximum amount of DescriptorSets that can be allocated from this pool. */
        };

        //FUNCTIONS
    public:
        virtual ~GhaDescriptorPool() = default;

        virtual Descriptor const &getDescriptor() const = 0;

        /** 
         * @brief Allocates a descriptor set for each layout provided.
         */
        virtual std::unique_ptr<GhaDescriptorSet> allocateDescriptorSets(GhaDescriptorSetLayout const *const layout)                              = 0;
        virtual std::vector<std::unique_ptr<GhaDescriptorSet>> allocateDescriptorSets(std::vector<GhaDescriptorSetLayout const *> const &layouts) = 0;

        /**
         * @brief Free an individual descriptor set. Requires Flag::FreeDescriptorSet to be set on creation.
         */
        virtual void freeDescriptorSets(std::unique_ptr<GhaDescriptorSet> &descriptorSet) = 0;
        /**
         * @brief Frees individual descriptor sets. Requires Flag::FreeDescriptorSet to be set on creation.
         */
        virtual void freeDescriptorSets(std::vector<std::unique_ptr<GhaDescriptorSet>> &descriptorSets) = 0;

        /**
         * @brief Resets this pool interally freeing all DescriptorSets allocated from it.
         * @details This will only internally reset the descriptor pool and not reset any pointers. 
         * All sets allocated from this pool will be in an undefined state and must be deleted manually.
         */
        virtual void reset() = 0;
    };
}
