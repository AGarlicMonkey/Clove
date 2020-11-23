#pragma once

namespace clv::gfx {
    enum class CommandBufferUsage {
        Default,       /**< Command buffer will be recorded and used mutliple times before rerecording. */
        OneTimeSubmit, /**< Command buffer will be recorded and used only once before rerecording. */
    };
}
