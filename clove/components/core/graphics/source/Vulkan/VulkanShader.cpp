#include "Clove/Graphics/Vulkan/VulkanShader.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>
#include <vector>

namespace clove {
    VulkanShader::VulkanShader(DevicePointer device, VkShaderModule module)
        : device{ std::move(device) }
        , module{ module } {
    }

    VulkanShader::VulkanShader(VulkanShader &&other) noexcept = default;

    VulkanShader &VulkanShader::operator=(VulkanShader &&other) noexcept = default;

    VulkanShader::~VulkanShader() {
        vkDestroyShaderModule(device.get(), module, nullptr);
    }

    VkShaderModule VulkanShader::getModule() const {
        return module;
    }

    VkShaderStageFlags VulkanShader::convertStage(Stage stage) {
        VkShaderStageFlags vkStage{ 0 };
        if((stage & GhaShader::Stage::Vertex) != 0) {
            vkStage |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        if((stage & GhaShader::Stage::Pixel) != 0) {
            vkStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        if((stage & GhaShader::Stage::Compute) != 0) {
            vkStage |= VK_SHADER_STAGE_COMPUTE_BIT;
        }
        return vkStage;
    }
}
