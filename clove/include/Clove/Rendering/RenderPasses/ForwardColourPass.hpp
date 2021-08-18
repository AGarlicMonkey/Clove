#pragma once

#include "Clove/Rendering/RenderPasses/GeometryPass.hpp"

#include <memory>

namespace clove {
    class GhaFactory;
    class GhaGraphicsPipelineObject;
    class GhaRenderPass;
}

namespace clove {
    class ForwardColourPass : public GeometryPass {
        //VARIABLES
    private:
        std::unique_ptr<GhaGraphicsPipelineObject> pipeline{ nullptr };

        //FUNCTIONS
    public:
        ForwardColourPass() = delete;
        ForwardColourPass(GhaFactory &ghaFactory, std::shared_ptr<GhaRenderPass> ghaRenderPass);//TEMP: Using an external render pass for now but these pass will need to create their own

        ForwardColourPass(ForwardColourPass const &other) = delete;
        ForwardColourPass(ForwardColourPass &&other) noexcept;

        ForwardColourPass &operator=(ForwardColourPass const &other) = delete;
        ForwardColourPass &operator=(ForwardColourPass &&other) noexcept;

        ~ForwardColourPass();

        void execute(GhaGraphicsCommandBuffer &commandBuffer, FrameData const &frameData) override;
    };
}