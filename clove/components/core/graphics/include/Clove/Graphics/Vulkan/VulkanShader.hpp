#pragma once

#include "Clove/Graphics/GhaShader.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <span>
#include <string_view>
#include <vulkan/vulkan.h>

namespace clove {
    class VulkanShader : public GhaShader {
        //VARIABLES
    private:
        DevicePointer device;

        VkShaderModule module{ VK_NULL_HANDLE };

        //FUNCTIONS
    public:
        VulkanShader() = delete;
        VulkanShader(DevicePointer device, VkShaderModule module);

        VulkanShader(VulkanShader const &other) = delete;
        VulkanShader(VulkanShader &&other) noexcept;

        VulkanShader &operator=(VulkanShader const &other) = delete;
        VulkanShader &operator=(VulkanShader &&other) noexcept;

        ~VulkanShader();

        VkShaderModule getModule() const;

        static VkShaderStageFlags convertStage(Stage stage);
    };
}