#pragma once

#include "Clove/Graphics/Result.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    Result convertResult(VkResult result);
}