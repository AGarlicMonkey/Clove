#pragma once

#include "Clove/Graphics/GhaDescriptorPool.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    class VulkanDescriptorPool : public GhaDescriptorPool {
        //VARIABLES
    private:
        DevicePointer device;
        VkDescriptorPool pool{ VK_NULL_HANDLE };

        Descriptor descriptor;

        //FUNCTIONS
    public:
        VulkanDescriptorPool() = delete;
        VulkanDescriptorPool(DevicePointer device, VkDescriptorPool pool, Descriptor descriptor);

        VulkanDescriptorPool(VulkanDescriptorPool const &other) = delete;
        VulkanDescriptorPool(VulkanDescriptorPool &&other) noexcept;

        VulkanDescriptorPool &operator=(VulkanDescriptorPool const &other) = delete;
        VulkanDescriptorPool &operator=(VulkanDescriptorPool &&other) noexcept;

        ~VulkanDescriptorPool();

        inline Descriptor const &getDescriptor() const override;

        std::shared_ptr<GhaDescriptorSet> allocateDescriptorSets(std::shared_ptr<GhaDescriptorSetLayout> const &layout) override;
        std::vector<std::shared_ptr<GhaDescriptorSet>> allocateDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSetLayout>> const &layouts) override;

        void freeDescriptorSets(std::shared_ptr<GhaDescriptorSet> const &descriptorSet) override;
        void freeDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSet>> const &descriptorSets) override;

        void reset() override;
    };
}

#include "VulkanDescriptorPool.inl"