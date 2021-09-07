#pragma once

#include "Clove/Graphics/Metal/MetalView.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
    struct DeviceWrapper {
        id<MTLDevice> device{ nullptr };
        MetalView *view{ nullptr };
    };
}