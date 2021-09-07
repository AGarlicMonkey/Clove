#include "Clove/Graphics/Gha.hpp"

#if CLOVE_PLATFORM_WINDOWS
    #include "Clove/Graphics/Vulkan/VulkanDevice.hpp"
    #include "Clove/Graphics/Vulkan/VulkanGha.hpp"
#elif CLOVE_PLATFORM_MACOS
    #include "Clove/Graphics/Metal/MetalDevice.hpp"
    #include "Clove/Graphics/Metal/MetalGha.hpp"
#elif CLOVE_PLATFORM_LINUX
    #include "Clove/Graphics/Vulkan/VulkanDevice.hpp"
    #include "Clove/Graphics/Vulkan/VulkanGha.hpp"
#endif

#if CLOVE_GHA_VALIDATION
    #include "Clove/Graphics/Validation/ValidationLog.hpp"
#endif

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveGha)

namespace clove {
    Expected<std::unique_ptr<GhaDevice>, std::runtime_error> createGhaDevice(GraphicsApi api, std::any nativeWindow) noexcept {
#if CLOVE_GHA_VALIDATION
        CLOVE_LOG(CloveGhaValidation, LogLevel::Debug, "GHA validation enabled.");
#endif

        std::unique_ptr<GhaDevice> device{ nullptr };
        switch(api) {
#if CLOVE_PLATFORM_WINDOWS
            case GraphicsApi::Vulkan: {
                auto result{ createVulkanDevice(std::move(nativeWindow)) };

                if(!result.hasValue()) {
                    return Unexpected{ std::move(result.getError()) };
                } else {
                    device = std::move(result.getValue());
                }
            } break;
#elif CLOVE_PLATFORM_MACOS
            case GraphicsApi::Metal: {
                auto result{ createMetalDevice(std::move(nativeWindow)) };

                if(!result.hasValue()) {
                    return Unexpected{ std::move(result.getError()) };
                } else {
                    device = std::move(result.getValue());
                }
            } break;
#elif CLOVE_PLATFORM_LINUX
            case GraphicsApi::Vulkan: {
                auto result{ createVulkanDevice(std::move(nativeWindow)) };

                if(!result.hasValue()) {
                    return Unexpected{ std::move(result.getError()) };
                } else {
                    device = std::move(result.getValue());
                }
            } break;
#endif
            default:
                CLOVE_ASSERT_MSG(false, "{0}: Default statement hit. Could not create GHA device.", CLOVE_FUNCTION_NAME);
                break;
        }

        GhaDevice::Info const info{ device->getInfo() };

        CLOVE_LOG(CloveGha, LogLevel::Trace, "GHA device created with a {0} backend.", info.ApiName);
        if(info.ApiVersion.has_value()) {
            CLOVE_LOG(CloveGha, LogLevel::Trace, "\tAPI:\t{0}.{1}.{2}", info.ApiVersion->major, info.ApiVersion->minor, info.ApiVersion->patch);
        }
        CLOVE_LOG(CloveGha, LogLevel::Trace, "\tDevice:\t{0}", info.deviceName);
        if(info.driverVersion.has_value()) {
            CLOVE_LOG(CloveGha, LogLevel::Trace, "\tDriver:\t{0}.{1}.{2}", info.driverVersion->major, info.driverVersion->minor, info.driverVersion->patch);
        }

        return device;
    }
}
