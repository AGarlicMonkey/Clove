#pragma once

#include "Clove/Graphics/PipelineObject.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
	MTLRenderStages convertStage(PipelineStage stage);
}
