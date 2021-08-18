#pragma once

#if CLOVE_PLATFORM_WINDOWS
    #include "Clove/Platform/Windows/CloveWindows.hpp"
#endif

#include <ostream>

namespace clove {
    enum class MouseButton {
        None      = -2,
        Undefined = -1,

#if CLOVE_PLATFORM_WINDOWS
        _1 = MK_LBUTTON,
        _2 = MK_RBUTTON,
        _3 = MK_MBUTTON,
        _4 = MK_XBUTTON1,
        _5 = MK_XBUTTON2,
#elif CLOVE_PLATFORM_LINUX
        _1 = 1,
        _2 = 2,
        _3 = 3,
        _4 = 8,
        _5 = 9,
#elif CLOVE_PLATFORM_MACOS
        _1 = 1 << 0,
        _2 = 1 << 1,
        _3 = 1 << 2,
        _4 = 1 << 3,
        _5 = 1 << 4,
#endif

        Left   = _1,
        Right  = _2,
        Middle = _3,
    };

#if CLOVE_PLATFORM_WINDOWS
    #define CLV_WHEEL_DELTA WHEEL_DELTA
#elif CLOVE_PLATFORM_LINUX
    #define CLV_WHEEL_DELTA 1
#elif CLOVE_PLATFORM_MACOS
    #define CLV_WHEEL_DELTA 1
#endif

    inline std::ostream &operator<<(std::ostream &os, MouseButton button) {
        return os << static_cast<int32_t>(button);
    }
}
