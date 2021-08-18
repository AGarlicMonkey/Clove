#include "Clove/Rendering/Techniques/ForwardLightingTechnique.hpp"

#include "Clove/Rendering/RenderPasses/ForwardColourPass.hpp"
#include "Clove/Rendering/RenderPasses/DirectionalLightPass.hpp"
#include "Clove/Rendering/RenderPasses/PointLightPass.hpp"
#include "Clove/Rendering/RenderPasses/SkinningPass.hpp"

namespace clove {
    Technique createForwardLightingTechnique() {
        return Technique{
            .passIds = std::set<GeometryPass::Id>{
                GeometryPass::getId<ForwardColourPass>(),
                GeometryPass::getId<DirectionalLightPass>(),
                GeometryPass::getId<PointLightPass>()
            },
        };
    }

    Technique createSkinnedForwardLightingTechnique(){
        Technique technique{ createForwardLightingTechnique() };
        technique.passIds.emplace(GeometryPass::getId<SkinningPass>());

        return technique;
    }
}