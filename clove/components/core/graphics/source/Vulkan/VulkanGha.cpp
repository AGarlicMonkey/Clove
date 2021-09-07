#include "Clove/Graphics/Vulkan/VulkanGha.hpp"

#if CLOVE_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif CLOVE_PLATFORM_MACOS
#elif CLOVE_PLATFORM_LINUX
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

//Vulkan needs to be before everything otherwise header guards from other headers will prevent compilation
#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.h>
//~~~~

//Remove xlib defs from vulkan.h
#if CLOVE_PLATFORM_LINUX
    #undef None
    #undef Success
#endif

#include "Clove/Graphics/Vulkan/VulkanDevice.hpp"
#include "Clove/Graphics/Vulkan/VulkanFactory.hpp"
#include "Clove/Graphics/Vulkan/VulkanLog.hpp"

#include <Clove/Definitions.hpp>
#include <set>
#include <sstream>
#include <unordered_set>

CLOVE_DECLARE_LOG_CATEGORY(Vulkan)

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messagType,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
    void *pUserData) {
    if((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0) {
        CLOVE_LOG(Vulkan, clove::LogLevel::Trace, pCallbackData->pMessage);
    } else if((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0) {
        CLOVE_LOG(Vulkan, clove::LogLevel::Warning, pCallbackData->pMessage);
    } else if((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) {
        CLOVE_LOG(Vulkan, clove::LogLevel::Error, pCallbackData->pMessage);
    }

    return VK_FALSE;
}

namespace clove {
    namespace {
        VkResult createDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo, VkAllocationCallbacks const *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
            auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
            if(func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        bool checkValidationLayerSupport(std::vector<char const *> const &validationLayers) {
            uint32_t layerCount{ 0 };
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for(char const *layerName : validationLayers) {
                bool layerFound = false;

                for(auto const &layerProperties : availableLayers) {
                    if(std::strcmp(layerName, layerProperties.layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if(!layerFound) {
                    return false;
                }
            }

            return true;
        }

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "Generating queue family indicies...");

            QueueFamilyIndices indices{};
            std::set<uint32_t> graphicsFamilyIndices{};
            std::set<uint32_t> presentFamilyIndices{};
            std::set<uint32_t> transferFamilyIndices{};
            std::set<uint32_t> computeFamilyIndices{};

            uint32_t queueFamilyCount{ 0 };
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "{0} queue families available", queueFamilyCount);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            for(uint32_t i{ 0 }; auto const &queueFamily : queueFamilies) {
                //If we have a surface, check if we have presentation support. Otherwise we're running headless
                if(surface != VK_NULL_HANDLE) {
                    VkBool32 presentSupport{ VK_FALSE };
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
                    if(presentSupport == VK_TRUE) {
                        presentFamilyIndices.emplace(i);
                    }
                }

                if((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u) {
                    graphicsFamilyIndices.emplace(i);
                }
                if((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0u) {
                    transferFamilyIndices.emplace(i);
                }
                if((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u) {
                    computeFamilyIndices.emplace(i);
                }

                ++i;
            }

            if((presentFamilyIndices.empty() && surface != VK_NULL_HANDLE) || graphicsFamilyIndices.empty() || transferFamilyIndices.empty() || computeFamilyIndices.empty()) {
                CLOVE_LOG(CloveGhaVulkan, LogLevel::Error, "Some queue types are unsupported. {0} failed.", CLOVE_FUNCTION_NAME);
            }

            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "Distributing family types. Aiming for a unique family per queue type...");
            //Just taking the first one available for now. Not forcing the present queue to be unique for now.
            //TODO: Should just make graphics and present the same one?
            if(surface != VK_NULL_HANDLE) {
                indices.presentFamily = *presentFamilyIndices.begin();
            }

            indices.graphicsFamily = *graphicsFamilyIndices.begin();
            if(transferFamilyIndices.size() > 1) {
                transferFamilyIndices.erase(*indices.graphicsFamily);
            }
            if(computeFamilyIndices.size() > 1) {
                computeFamilyIndices.erase(*indices.graphicsFamily);
            }

            indices.transferFamily = *transferFamilyIndices.begin();
            if(computeFamilyIndices.size() > 1) {
                computeFamilyIndices.erase(*indices.transferFamily);
            }

            indices.computeFamily = *computeFamilyIndices.begin();

            CLOVE_LOG(CloveGhaVulkan, LogLevel::Debug, "Vulkan queue types successfully distributed!");
            if(surface != VK_NULL_HANDLE) {
                CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "\tPresent:\tid: {0}, count: {1}", *indices.presentFamily, queueFamilies[*indices.presentFamily].queueCount);
            }
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "\tGraphics:\tid: {0}, count: {1}", *indices.graphicsFamily, queueFamilies[*indices.graphicsFamily].queueCount);
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "\tTransfer:\tid: {0}, count: {1}", *indices.transferFamily, queueFamilies[*indices.transferFamily].queueCount);
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "\tCompute:\tid: {0}, count: {1}", *indices.computeFamily, queueFamilies[*indices.computeFamily].queueCount);

            return indices;
        }

        bool checkDeviceExtensionsSupport(VkPhysicalDevice device, std::vector<char const *> const &extensions) {
            uint32_t extensionCount{ 0 };
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            for(char const *extensionName : extensions) {
                bool found = false;

                for(auto const &extensionProperties : availableExtensions) {
                    if(std::strcmp(extensionName, extensionProperties.extensionName) == 0) {
                        found = true;
                        break;
                    }
                }

                if(!found) {
                    return false;
                }
            }

            return true;
        }

        bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<char const *> const &extensions) {
            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            VkPhysicalDeviceProperties devicePoperties{};
            vkGetPhysicalDeviceProperties(device, &devicePoperties);

            QueueFamilyIndices indices{ findQueueFamilies(device, surface) };
            bool const extentionsAreSupported{ checkDeviceExtensionsSupport(device, extensions) };

            std::optional<bool> surfaceIsAdequate{};
            if(extentionsAreSupported) {
                //If we don't have a surface then we're headless
                if(surface != VK_NULL_HANDLE) {
                    SurfaceSupportDetails const surfaceSupport{ SurfaceSupportDetails::query(device, surface) };
                    //We'll consider the surface adequate if we have one supported image format and presentation mode
                    surfaceIsAdequate = !surfaceSupport.formats.empty() && !surfaceSupport.presentModes.empty();
                }
            } else {
                surfaceIsAdequate = false;
            }

            bool const requirePresentFamily{ surface != VK_NULL_HANDLE };
            bool const hasFeatures{ deviceFeatures.samplerAnisotropy == VK_TRUE && deviceFeatures.imageCubeArray == VK_TRUE };
            return indices.isComplete(requirePresentFamily) && extentionsAreSupported && surfaceIsAdequate.value_or(true) && hasFeatures && devicePoperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        }
    }

    Expected<std::unique_ptr<VulkanDevice>, std::runtime_error> createVulkanDevice(std::any nativeWindow) noexcept {
        std::vector<char const *> deviceExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        std::vector<char const *> requiredExtensions {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if CLOVE_PLATFORM_WINDOWS
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif CLOVE_PLATFORM_MACOS
#elif CLOVE_PLATFORM_LINUX
                VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif
#if CLOVE_GHA_VALIDATION
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        };

#if CLOVE_GHA_VALIDATION
        std::vector<char const *> const validationLayers{
            "VK_LAYER_KHRONOS_validation"
        };

        if(!checkValidationLayerSupport(validationLayers)) {
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Warning, "Vulkan validation layers are not supported on this device. Unable to provide debugging infomation");
        }
#endif

        //Create instance
        VkInstance instance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };
        {
            VkApplicationInfo appInfo{
                .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName   = "",                      //TODO: Get app name
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),//TODO: Get app version
                .pEngineName        = "Clove",
                .engineVersion      = VK_MAKE_VERSION(1, 0, 0),//TODO: Get engine version
                .apiVersion         = VK_API_VERSION_1_2,
            };

#if CLOVE_GHA_VALIDATION
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
                .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = debugCallback,
                .pUserData       = nullptr,
            };
#endif

            VkInstanceCreateInfo createInfo {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if CLOVE_GHA_VALIDATION
                .pNext                = &debugMessengerCreateInfo,//Setting the pNext allows us to debug the creation and destruction of the instance (as normaly we need an instance pointer to enable debugging)
                    .pApplicationInfo = &appInfo,
                .enabledLayerCount    = static_cast<uint32_t>(std::size(validationLayers)),
                .ppEnabledLayerNames  = std::data(validationLayers),
#else
                .pNext               = nullptr,
                .pApplicationInfo    = &appInfo,
                .enabledLayerCount   = 0,
                .ppEnabledLayerNames = nullptr,
#endif
                .enabledExtensionCount   = static_cast<uint32_t>(std::size(requiredExtensions)),
                .ppEnabledExtensionNames = std::data(requiredExtensions),
            };

            if(VkResult const result{ vkCreateInstance(&createInfo, nullptr, &instance) }; result != VK_SUCCESS) {
                switch(result) {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the VK instance. Out of host memory." } };
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the VK instance. Out of device memory." } };
                    case VK_ERROR_INITIALIZATION_FAILED:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the VK instance. Initialisation failed." } };
                    case VK_ERROR_LAYER_NOT_PRESENT:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the VK instance. One or more requested layers are not present." } };
                    case VK_ERROR_EXTENSION_NOT_PRESENT:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the VK instance. One or more requested extensions are not present." } };
                    case VK_ERROR_INCOMPATIBLE_DRIVER:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the VK instance. Driver is in compatible." } };
                    default:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the VK instance. Reson unknown." } };
                }
            }

#if CLOVE_GHA_VALIDATION
            if(createDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
                CLOVE_LOG(CloveGhaVulkan, LogLevel::Warning, "Failed to create debug messenger. Debug info will be limited.");
            }
#endif
        }

        //Create surface
        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        if(nativeWindow.has_value()) {
#if CLOVE_PLATFORM_WINDOWS
            VkWin32SurfaceCreateInfoKHR createInfo{
                .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                .hinstance = GetModuleHandle(nullptr),
                .hwnd      = std::any_cast<HWND>(nativeWindow),
            };

            if(VkResult const result{ vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) }; result != VK_SUCCESS) {
                switch(result) {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the windows surface. Out of host memory." } };
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the windows surface. Out of device memory." } };
                    default:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the windows surface. Reson unknown." } };
                }
            }
#elif CLOVE_PLATFORM_MACOS
            CLOVE_ASSERT_MSG(false, "Vulkan implementation not provided on MacOS");
#elif CLOVE_PLATFORM_LINUX
            auto const [display, window] = std::any_cast<std::pair<Display *, ::Window>>(nativeWindow);

            VkXlibSurfaceCreateInfoKHR createInfo{
                .sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
                .dpy    = display,
                .window = window,
            };

            if(VkResult const result{ vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface) }; result != VK_SUCCESS) {
                switch(result) {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the xlib surface. Out of host memory." } };
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the xlib surface. Out of device memory." } };
                    default:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the xlib surface. Reson unknown." } };
                }
            }
#endif
        }

        //Pick physical device
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        {
            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "Searching for a suitable physical device...");

            uint32_t deviceCount{ 0 };
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
            if(deviceCount == 0) {
                return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice. Could not find any GPUs with Vulkan support." } };
            }
            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "Found {0} potential devices:", deviceCount);
            for(auto const &device : devices) {
                VkPhysicalDeviceProperties devicePoperties{};
                vkGetPhysicalDeviceProperties(device, &devicePoperties);
                CLOVE_LOG(CloveGhaVulkan, LogLevel::Trace, "\t{0}", devicePoperties.deviceName);
            }

            for(auto const &device : devices) {
                if(isDeviceSuitable(device, surface, deviceExtensions)) {
                    physicalDevice = device;
                    break;
                }
            }

            if(physicalDevice == VK_NULL_HANDLE) {
                return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice. Could not find a suitable GPU." } };
            }
        }

        QueueFamilyIndices queueFamilyIndices{ findQueueFamilies(physicalDevice, surface) };

        //Create logical device
        VkDevice logicalDevice{ nullptr };
        {
            std::set<uint32_t> uniqueQueueFamilies{
                *queueFamilyIndices.graphicsFamily,
                *queueFamilyIndices.transferFamily,
                *queueFamilyIndices.computeFamily,
            };
            if(queueFamilyIndices.presentFamily.has_value()) {
                uniqueQueueFamilies.emplace(*queueFamilyIndices.presentFamily);
            }

            float constexpr queuePriority{ 1.0f };
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
            for(uint32_t queueFamily : uniqueQueueFamilies) {
                VkDeviceQueueCreateInfo queueCreateinfo{
                    .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = queueFamily,
                    .queueCount       = 1,
                    .pQueuePriorities = &queuePriority,
                };
                queueCreateInfos.push_back(queueCreateinfo);
            }

            //Sepcify our device features
            VkPhysicalDeviceFeatures deviceFeatures{
                .imageCubeArray    = VK_TRUE,
                .samplerAnisotropy = VK_TRUE,
            };

            VkDeviceCreateInfo createInfo {
                .sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                .pQueueCreateInfos    = queueCreateInfos.data(),

#if CLOVE_GHA_VALIDATION
                //We don't need to do this as device specific validation layers are no more. But seeing as it's the same data we can reuse them to support older versions
                    .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
                .ppEnabledLayerNames   = validationLayers.data(),
#else
                .enabledLayerCount = 0,
#endif
                .enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size()),
                .ppEnabledExtensionNames = deviceExtensions.data(),
                .pEnabledFeatures        = &deviceFeatures,
            };

            if(VkResult const result{ vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) }; result != VK_SUCCESS) {
                switch(result) {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. Out of host memory." } };
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. Out of device memory." } };
                    case VK_ERROR_INITIALIZATION_FAILED:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. Initialisation failed." } };
                    case VK_ERROR_EXTENSION_NOT_PRESENT:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. One or more requested extensions are not present." } };
                    case VK_ERROR_FEATURE_NOT_PRESENT:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. One or more requested features are not present." } };
                    case VK_ERROR_TOO_MANY_OBJECTS:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. Too many objects have been created." } };
                    case VK_ERROR_DEVICE_LOST:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. Device has been lost." } };
                    default:
                        return Unexpected{ std::runtime_error{ "Failed to create VulkanDevice when creating the logical device. Reson unknown." } };
                }
            }
        }

        DevicePointer devicePtr{ instance, surface, physicalDevice, logicalDevice, debugMessenger };
        auto factory{ std::make_unique<VulkanFactory>(devicePtr, queueFamilyIndices) };

        return std::make_unique<VulkanDevice>(std::move(devicePtr), std::move(factory));
    }
}