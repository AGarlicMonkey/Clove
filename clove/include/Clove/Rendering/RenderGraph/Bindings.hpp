#pragma once

#include "Clove/Rendering/RenderGraph/RgId.hpp"

#include <Clove/Graphics/GhaGraphicsCommandBuffer.hpp>
#include <Clove/Graphics/GhaRenderPass.hpp>
#include <Clove/Graphics/GhaSampler.hpp>
#include <Clove/Graphics/GhaShader.hpp>

namespace clove {
    struct RgBufferBinding {
        uint32_t slot{};
        RgBufferId buffer{};
        size_t offset{}; /**< Offset into the buffer to start binding from. */
        size_t size{};   /**< Size of the binding within the buffer. */

        GhaShader::Stage shaderStage{};//TEMP: Manually set shader stage in place of shader reflection.
    };

    struct RgImageBinding {
        uint32_t slot{};
        RgImageViewId image{};
    };

    struct RgSamplerBinding {
        uint32_t slot{};
        RgResourceId sampler{};
    };

    struct RgRenderTargetBinding {
        LoadOperation loadOp{};
        StoreOperation storeOp{};
        ColourValue clearColour{};
        RgImageViewId target{};
    };

    struct RgDepthStencilBinding {
        LoadOperation loadOp{};
        StoreOperation storeOp{};
        DepthStencilValue clearValue{};
        RgImageViewId target{};
    };
}