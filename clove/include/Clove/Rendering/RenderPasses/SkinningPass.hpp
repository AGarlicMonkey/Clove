#pragma once

#include "Clove/Rendering/RenderPasses/GeometryPass.hpp"

#include <memory>

namespace clove {
    class GhaComputePipelineObject;
    class GhaFactory;
}

namespace clove {
    class SkinningPass : public GeometryPass {
        //VARIABLES
    private:
        std::unique_ptr<GhaComputePipelineObject> pipeline;

        //FUNCTIONS
    public:
        SkinningPass() = delete;
        SkinningPass(GhaFactory &ghaFactory);

        SkinningPass(SkinningPass const &other) = delete;
        SkinningPass(SkinningPass &&other) noexcept;

        SkinningPass &operator=(SkinningPass const &other) = delete;
        SkinningPass &operator=(SkinningPass &&other) noexcept;

        ~SkinningPass();

        void execute(GhaComputeCommandBuffer &commandBuffer, FrameData const &frameData) override;
    };
}