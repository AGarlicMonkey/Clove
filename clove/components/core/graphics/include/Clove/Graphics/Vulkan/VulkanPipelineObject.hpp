#pragma once

#include "Clove/Graphics/PipelineObject.hpp"

#include <vulkan/vulkan.h>

namespace clove {
    VkPipelineStageFlags convertStage(PipelineStage stage);
}