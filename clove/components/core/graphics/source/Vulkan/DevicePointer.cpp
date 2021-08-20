#include "Clove/Graphics/Vulkan/DevicePointer.hpp"

#include <utility>

namespace clove {
    namespace {
        void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
            auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
            if(func != nullptr) {
                func(instance, debugMessenger, pAllocator);
            }
        }
    }

    DevicePointer::DevicePointer() = default;

    DevicePointer::DevicePointer(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDebugUtilsMessengerEXT debugMessenger)
        : instance(instance)
        , surface(surface)
        , physicalDevice(physicalDevice)
        , logicalDevice(logicalDevice)
        , debugMessenger(debugMessenger) {
        counter = new uint32_t(1);
    }

    DevicePointer::DevicePointer(DevicePointer const &other) {
        release();

        instance       = other.instance;
        surface        = other.surface;
        physicalDevice = other.physicalDevice;
        logicalDevice  = other.logicalDevice;
        debugMessenger = other.debugMessenger;

        counter = other.counter;
        if(counter != nullptr) {
            ++(*counter);
        }
    }

    DevicePointer::DevicePointer(DevicePointer &&other) noexcept {
        release();

        instance       = other.instance;
        surface        = other.surface;
        physicalDevice = other.physicalDevice;
        logicalDevice  = other.logicalDevice;
        debugMessenger = other.debugMessenger;

        counter       = other.counter;
        other.counter = nullptr;
    }

    DevicePointer &DevicePointer::operator=(DevicePointer const &other) {
        release();

        instance       = other.instance;
        surface        = other.surface;
        physicalDevice = other.physicalDevice;
        logicalDevice  = other.logicalDevice;
        debugMessenger = other.debugMessenger;

        counter = other.counter;
        if(counter != nullptr) {
            ++(*counter);
        }

        return *this;
    }

    DevicePointer &DevicePointer::operator=(DevicePointer &&other) noexcept {
        release();

        instance       = other.instance;
        surface        = other.surface;
        physicalDevice = other.physicalDevice;
        logicalDevice  = other.logicalDevice;
        debugMessenger = other.debugMessenger;

        counter       = other.counter;
        other.counter = nullptr;

        return *this;
    }

    DevicePointer::~DevicePointer() {
        release();
    }

    void DevicePointer::release() {
        if(counter != nullptr && --(*counter) == 0) {
#if CLOVE_GHA_VALIDATION
            destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif

            vkDestroyDevice(logicalDevice, nullptr);
            if(surface != VK_NULL_HANDLE) {
                vkDestroySurfaceKHR(instance, surface, nullptr);
            }
            vkDestroyInstance(instance, nullptr);

            delete counter;
            counter = nullptr;
        }
    }
}