#pragma once

#include "Clove/Graphics/GhaDevice.hpp"
#include "Clove/Graphics/Vulkan/DevicePointer.hpp"
#include "Clove/Graphics/Vulkan/VulkanTypes.hpp"

#include <any>
#include <memory>

namespace clove {
    class VulkanFactory;
}

namespace clove {
    class VulkanDevice : public GhaDevice {
        //VARIABLES
    private:
        DevicePointer devicePtr;

        std::shared_ptr<VulkanFactory> factory;

        //FUNCTIONS
    public:
        VulkanDevice() = delete;
        VulkanDevice(std::any nativeWindow);

        VulkanDevice(VulkanDevice const &other) = delete;
        VulkanDevice(VulkanDevice &&other) noexcept;

        VulkanDevice &operator=(VulkanDevice const &other) = delete;
        VulkanDevice &operator=(VulkanDevice &&other) noexcept;

        ~VulkanDevice();

        std::shared_ptr<GhaFactory> getGraphicsFactory() const override;

        void waitForIdleDevice() override;

        Limits getLimits() const override;
    };
}