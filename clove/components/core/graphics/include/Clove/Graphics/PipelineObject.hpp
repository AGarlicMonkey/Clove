#pragma once

#include "Clove/Graphics/GhaShader.hpp"

#include <cstddef>

namespace clove {
    enum class PipelineStage {
        //External Stages
        Top,

        //Transfer stages
        Transfer,

        //Compute stages
        ComputeShader,

        //Graphics stages
        VertexInput,
        VertexShader,
        PixelShader,
        EarlyPixelTest,
        LatePixelTest,
        ColourAttachmentOutput,
    };

    struct PushConstantDescriptor {
        GhaShader::Stage stage;
        size_t offset{ 0 }; /**< Offset specified in the shader (layout(offset = x)). Required if using different push constants for different stages. */
        size_t size{ 0 };
    };
}