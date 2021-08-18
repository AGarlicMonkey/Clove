#pragma once

#include <vulkan/vulkan.h>

namespace clove {
    /**
     * @brief Wraps a Vulkan device / instance to help control the lifetime of those objects.
     */
    class DevicePointer {
        //VARIABLES
    private:
        VkDevice logicalDevice{ VK_NULL_HANDLE };
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

        VkInstance instance{ VK_NULL_HANDLE };
        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };

        uint32_t *counter{ nullptr };

        //FUNCTIONS
    public:
        DevicePointer();
        DevicePointer(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDebugUtilsMessengerEXT debugMessenger);

        DevicePointer(DevicePointer const &other);
        DevicePointer(DevicePointer &&other) noexcept;

        DevicePointer &operator=(DevicePointer const &other);
        DevicePointer &operator=(DevicePointer &&other) noexcept;

        ~DevicePointer();

        inline VkDevice get() const;
        inline VkPhysicalDevice getPhysical() const;

        inline VkSurfaceKHR getSurface() const;

    private:
        void release();
    };
}

#include "DevicePointer.inl"