#pragma once

#include "Clove/Rendering/RenderingConstants.hpp"

#include <Clove/Graphics/Descriptor.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <unordered_map>
#include <filesystem>

namespace garlic::clove {
    class GhaDescriptorSetLayout;
    class GhaFactory;
}

namespace garlic::clove {
    //TODO: Remove
    /**
     * @brief Creates all descriptor sets according to DescriptorSetSlots.
     */
    std::unordered_map<DescriptorSetSlots, std::shared_ptr<GhaDescriptorSetLayout>> createDescriptorSetLayouts(GhaFactory &factory);

    std::unique_ptr<GhaDescriptorSetLayout> createMeshDescriptorSetLayout(GhaFactory &factory);
    std::unique_ptr<GhaDescriptorSetLayout> createViewDescriptorSetLayout(GhaFactory &factory);
    std::unique_ptr<GhaDescriptorSetLayout> createLightingDescriptorSetLayout(GhaFactory &factory);
    std::unique_ptr<GhaDescriptorSetLayout> createUiDescriptorSetLayout(GhaFactory &factory);

    /**
     * @brief Counts the different binding types in a DescriptorSetLayout.
     * @returns A map where each key is the binding type and the value is the count.
     */
    std::unordered_map<DescriptorType, uint32_t> countDescriptorBindingTypes(GhaDescriptorSetLayout const &descriptorSetLayout);

    /**
     * @brief Creates an image and then transfers the data into it.
     */
    std::unique_ptr<GhaImage> createImageWithData(GhaFactory &factory, GhaImage::Descriptor imageDescriptor, void const *data, size_t const dataSize);

    /**
     * @brief Creates an image from the specified path
     */
    std::unique_ptr<GhaImage> createImageFromPath(GhaFactory &factory, std::filesystem::path const &path);
}