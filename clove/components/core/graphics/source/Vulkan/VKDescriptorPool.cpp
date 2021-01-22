#include "Clove/Graphics/Vulkan/VKDescriptorPool.hpp"

#include "Clove/Graphics/Vulkan/VKDescriptorSet.hpp"
#include "Clove/Graphics/Vulkan/VKDescriptorSetLayout.hpp"

#include <Clove/Cast.hpp>
#include <Clove/Log/Log.hpp>

namespace garlic::clove {
    VKDescriptorPool::VKDescriptorPool(DevicePointer device, VkDescriptorPool pool, Descriptor descriptor)
        : device{ std::move(device) }
        , pool{ pool }
        , descriptor{ std::move(descriptor) } {
    }

    VKDescriptorPool::VKDescriptorPool(VKDescriptorPool &&other) noexcept = default;

    VKDescriptorPool &VKDescriptorPool::operator=(VKDescriptorPool &&other) noexcept = default;

    VKDescriptorPool::~VKDescriptorPool() {
        vkDestroyDescriptorPool(device.get(), pool, nullptr);
    }

    std::shared_ptr<GhaDescriptorSet> VKDescriptorPool::allocateDescriptorSets(std::shared_ptr<GhaDescriptorSetLayout> const &layout) {
        return allocateDescriptorSets(std::vector{ layout })[0];
    }

    std::vector<std::shared_ptr<GhaDescriptorSet>> VKDescriptorPool::allocateDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSetLayout>> const &layouts) {
        size_t const numSets = std::size(layouts);

        std::vector<VkDescriptorSetLayout> vulkanLayouts(numSets);
        for(size_t i = 0; i < numSets; ++i) {
            vulkanLayouts[i] = polyCast<VKDescriptorSetLayout>(layouts[i].get())->getLayout();
        }

        std::vector<VkDescriptorSet> vulkanSets(numSets);

        VkDescriptorSetAllocateInfo allocInfo{
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext              = nullptr,
            .descriptorPool     = pool,
            .descriptorSetCount = static_cast<uint32_t>(numSets),
            .pSetLayouts        = std::data(vulkanLayouts),
        };

        if(vkAllocateDescriptorSets(device.get(), &allocInfo, std::data(vulkanSets)) != VK_SUCCESS) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Failed to allocate new descriptor sets");
            return {};
        }

        std::vector<std::shared_ptr<GhaDescriptorSet>> descriptorSets(numSets);
        for(size_t i = 0; i < numSets; ++i) {
            descriptorSets[i] = std::make_shared<VKDescriptorSet>(device.get(), vulkanSets[i]);
        }

        return descriptorSets;
    }

    void VKDescriptorPool::freeDescriptorSets(std::shared_ptr<GhaDescriptorSet> const &descriptorSet) {
        freeDescriptorSets(std::vector{ descriptorSet });
    }

    void VKDescriptorPool::freeDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSet>> const &descriptorSets) {
        size_t const numSets = std::size(descriptorSets);

        std::vector<VkDescriptorSet> vulkanSets(numSets);
        for(size_t i = 0; i < numSets; ++i) {
            vulkanSets[i] = polyCast<VKDescriptorSet>(descriptorSets[i].get())->getDescriptorSet();
        }

        if(vkFreeDescriptorSets(device.get(), pool, numSets, std::data(vulkanSets)) != VK_SUCCESS) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Failed to free descriptor sets");
        }
    }

    void VKDescriptorPool::reset() {
        vkResetDescriptorPool(device.get(), pool, 0);
    }
}