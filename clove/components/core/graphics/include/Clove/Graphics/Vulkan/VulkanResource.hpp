#pragma once

#include "Clove/Graphics/Resource.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    VkAccessFlags convertAccessFlags(AccessFlags garlicAccess);
}