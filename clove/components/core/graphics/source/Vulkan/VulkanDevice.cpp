#include "Clove/Graphics/Vulkan/VulkanDevice.hpp"

#include "Clove/Graphics/Vulkan/VulkanLog.hpp"

namespace clove {
    VulkanDevice::VulkanDevice(DevicePointer devicePtr, std::unique_ptr<VulkanFactory> factory)
        : devicePtr{ std::move(devicePtr) }
        , factory{ std::move(factory) } {
    }

    VulkanDevice::VulkanDevice(VulkanDevice &&other) noexcept = default;

    VulkanDevice &VulkanDevice::operator=(VulkanDevice &&other) noexcept = default;

    VulkanDevice::~VulkanDevice() = default;

    GhaFactory *VulkanDevice::getGraphicsFactory() const {
        return factory.get();
    }

    void VulkanDevice::waitForIdleDevice() {
        if(VkResult const result{ vkDeviceWaitIdle(devicePtr.get()) }; result != VK_SUCCESS) {
            switch(result) {
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                    CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Error while waiting for device. Out of host memory.");
                    break;
                case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                    CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Error while waiting for device. Out of device memory.");
                    break;
                case VK_ERROR_DEVICE_LOST:
                    CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Error while waiting for device. Device lost.");
                    break;
                default:
                    CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Unknown error while waiting for device.");
                    break;
            }
        }
    }

    GhaDevice::Info VulkanDevice::getInfo() const {
        VkPhysicalDeviceProperties devicePoperties;
        vkGetPhysicalDeviceProperties(devicePtr.getPhysical(), &devicePoperties);

        return Info{
            .ApiName       = "Vulkan",
            .deviceName    = devicePoperties.deviceName,
            .driverVersion = Version{
                .major = VK_VERSION_MAJOR(devicePoperties.driverVersion),
                .minor = VK_VERSION_MINOR(devicePoperties.driverVersion),
                .patch = VK_VERSION_PATCH(devicePoperties.driverVersion),
            },
            .ApiVersion = Version{
                .major = VK_VERSION_MAJOR(devicePoperties.apiVersion),
                .minor = VK_VERSION_MINOR(devicePoperties.apiVersion),
                .patch = VK_VERSION_PATCH(devicePoperties.apiVersion),
            },
        };
    }

    GhaDevice::Limits VulkanDevice::getLimits() const {
        VkPhysicalDeviceProperties devicePoperties;
        vkGetPhysicalDeviceProperties(devicePtr.getPhysical(), &devicePoperties);

        return Limits{
            .minUniformBufferOffsetAlignment = devicePoperties.limits.minUniformBufferOffsetAlignment
        };
    }
}
