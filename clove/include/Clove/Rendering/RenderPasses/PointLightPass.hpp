#pragma once

#include "Clove/Rendering/RenderPasses/GeometryPass.hpp"

#include <memory>

namespace clove {
    class GhaFactory;
    class GhaGraphicsPipelineObject;
    class GhaRenderPass;
}

namespace clove {
    class PointLightPass : public GeometryPass {
        //VARIABLES
    private:
        std::unique_ptr<GhaGraphicsPipelineObject> pipeline{ nullptr };

        //FUNCTIONS
    public:
        PointLightPass() = delete;
        PointLightPass(GhaFactory &ghaFactory, std::shared_ptr<GhaRenderPass> ghaRenderPass);//TEMP: Using an external render pass for now but these pass will need to create their own

        PointLightPass(PointLightPass const &other) = delete;
        PointLightPass(PointLightPass &&other) noexcept;

        PointLightPass &operator=(PointLightPass const &other) = delete;
        PointLightPass &operator=(PointLightPass &&other) noexcept;

        ~PointLightPass();

        void execute(GhaGraphicsCommandBuffer &commandBuffer, FrameData const &frameData) override;
    };
}