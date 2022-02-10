#pragma once

#include "Clove/Rendering/Animator.hpp"
#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/Renderables/AnimatedModel.hpp"

namespace clove {
    struct AnimatedModelComponent {
        AssetPtr<AnimatedModel> model;
        Material material{};
        Animator animator;
    };
}