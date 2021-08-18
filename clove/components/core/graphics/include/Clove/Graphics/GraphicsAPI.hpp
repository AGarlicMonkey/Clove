#pragma once

namespace clove {
    enum class GraphicsApi {
        None,
#if CLOVE_PLATFORM_WINDOWS
        Vulkan,
#elif CLOVE_PLATFORM_MACOS
        Metal,
#elif CLOVE_PLATFORM_LINUX
        Vulkan,
#endif
    };
}