#pragma once

#include "Clove/Graphics/Descriptor.hpp"

#include <vulkan/vulkan.h>

namespace clove{
    VkDescriptorType getDescriptorType(DescriptorType garlicType);
}