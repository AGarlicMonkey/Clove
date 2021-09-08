#pragma once

#include "Clove/Graphics/GhaDescriptorPool.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <vulkan/vulkan.h>
#include <vector>

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

        std::unique_ptr<GhaDescriptorSet> allocateDescriptorSets(GhaDescriptorSetLayout const *const layout) override;
        std::vector<std::unique_ptr<GhaDescriptorSet>> allocateDescriptorSets(std::vector<GhaDescriptorSetLayout const *> const &layouts) override;

        void freeDescriptorSets(std::unique_ptr<GhaDescriptorSet> &descriptorSet) override;
        void freeDescriptorSets(std::vector<std::unique_ptr<GhaDescriptorSet>> &descriptorSets) override;

        void reset() override;
    };
}

#include "VulkanDescriptorPool.inl"