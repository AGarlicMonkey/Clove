#include "Bulb/Rendering/RenderingHelpers.hpp"

#include <Clove/Graphics/DescriptorSetLayout.hpp>
#include <Clove/Graphics/GraphicsFactory.hpp>
#include <Clove/Graphics/GraphicsTypes.hpp>

namespace blb::rnd {
    std::vector<std::shared_ptr<clv::gfx::DescriptorSetLayout>> createDescriptorSetLayouts(clv::gfx::GraphicsFactory& factory) {
        std::shared_ptr<clv::gfx::DescriptorSetLayout> materialSetLayout;
        std::shared_ptr<clv::gfx::DescriptorSetLayout> viewSetLayout;
        std::shared_ptr<clv::gfx::DescriptorSetLayout> lightingSetLayout;

        //MATERIAL SET
        {
            clv::gfx::DescriptorSetBindingInfo samplerLayoutBinding{};
            samplerLayoutBinding.binding   = 0;
            samplerLayoutBinding.type      = clv::gfx::DescriptorType::CombinedImageSampler;
            samplerLayoutBinding.arraySize = 1;
            samplerLayoutBinding.stage     = clv::gfx::ShaderStage::Pixel;

            clv::gfx::DescriptorSetLayout::Descriptor materialSetLayoutDescriptor{};
            materialSetLayoutDescriptor.bindings = { samplerLayoutBinding };

            materialSetLayout = factory.createDescriptorSetLayout(materialSetLayoutDescriptor);
        }

        //VIEW SET
        {
            clv::gfx::DescriptorSetBindingInfo viewLayoutBinding{};
            viewLayoutBinding.binding   = 0;
            viewLayoutBinding.type      = clv::gfx::DescriptorType::UniformBuffer;
            viewLayoutBinding.arraySize = 1;
            viewLayoutBinding.stage     = clv::gfx::ShaderStage::Vertex;

            clv::gfx::DescriptorSetBindingInfo viewPosBinding{};
            viewPosBinding.binding   = 1;
            viewPosBinding.type      = clv::gfx::DescriptorType::UniformBuffer;
            viewPosBinding.arraySize = 1;
            viewPosBinding.stage     = clv::gfx::ShaderStage::Pixel;

            clv::gfx::DescriptorSetLayout::Descriptor viewSetLayoutDescriptor{};
            viewSetLayoutDescriptor.bindings = { viewLayoutBinding, viewPosBinding };

            viewSetLayout = factory.createDescriptorSetLayout(viewSetLayoutDescriptor);
        }

        //LIGHTING SET
        {
            clv::gfx::DescriptorSetBindingInfo lightDataBindingInfo{};
            lightDataBindingInfo.binding   = 0;
            lightDataBindingInfo.type      = clv::gfx::DescriptorType::UniformBuffer;
            lightDataBindingInfo.arraySize = 1;
            lightDataBindingInfo.stage     = clv::gfx::ShaderStage::Pixel;

            clv::gfx::DescriptorSetBindingInfo lightCountBindingInfo{};
            lightCountBindingInfo.binding   = 1;
            lightCountBindingInfo.type      = clv::gfx::DescriptorType::UniformBuffer;
            lightCountBindingInfo.arraySize = 1;
            lightCountBindingInfo.stage     = clv::gfx::ShaderStage::Pixel;

            clv::gfx::DescriptorSetLayout::Descriptor lightingSetLayoutDescriptor{};
            lightingSetLayoutDescriptor.bindings = { lightDataBindingInfo, lightCountBindingInfo };

            lightingSetLayout = factory.createDescriptorSetLayout(lightingSetLayoutDescriptor);
        }

        return { materialSetLayout, viewSetLayout, lightingSetLayout };
    }

    std::unordered_map<clv::gfx::DescriptorType, uint32_t> countDescriptorBindingTypes(const clv::gfx::DescriptorSetLayout& descriptorSetLayout) {
        std::unordered_map<clv::gfx::DescriptorType, uint32_t> counts;
        const auto& descriptor = descriptorSetLayout.getDescriptor();
        for(auto& binding : descriptor.bindings) {
            counts[binding.type]++;
        }

        return counts;
    }
}