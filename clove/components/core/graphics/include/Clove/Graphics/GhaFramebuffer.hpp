#pragma once

#include <vector>
#include <cinttypes>

namespace clove {
    class GhaRenderPass;
    class GhaImageView;
}

namespace clove {
    /**
     * @brief Provides attachments (as GraphicsImageViews) to a given GhaRenderPass.
     */
    class GhaFramebuffer {
        //TYPES
    public:
        struct Descriptor {
            GhaRenderPass const *renderPass{ nullptr };
            std::vector<GhaImageView const *> attachments; /**< The order of the attachments here has to match the those in the corresponding render pass. ColourAttachments + DepthStencilAttachment. */
            uint32_t width{ 0 };
            uint32_t height{ 0 };
        };

        //VARIABLES
    public:
        virtual ~GhaFramebuffer() = default;
    };
}
