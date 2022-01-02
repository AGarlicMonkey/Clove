#pragma once

#include "Clove/Graphics/GhaShader.hpp"

#include <cstddef>

namespace clove {
    enum class PipelineStage {
        //External Stages
        Top, /**< Before any following stages commence. */

        //Transfer stages
        Transfer, /**< All copy commands. */

        //Compute stages
        ComputeShader, /**< Execution of a compute shader. */

        //Graphics stages
        VertexInput,            /**< Where vertex and index buffers are consumed. */
        VertexShader,           /**< Execution of a vertex shader. */
        PixelShader,            /**< Execution of a pixel shader. */
        EarlyPixelTest,         /**< Early depth and stencil tests. */
        LatePixelTest,          /**< Late depth and stencil tests. */
        ColourAttachmentOutput, /**< After blending where the final colour values are output. This also includes depth attachments */
    };

    struct PushConstantDescriptor {
        GhaShader::Stage stage;
        size_t offset{ 0 }; /**< Offset specified in the shader (layout(offset = x)). Required if using different push constants for different stages. */
        size_t size{ 0 };
    };
}