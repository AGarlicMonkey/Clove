#pragma once

#include "Clove/Rendering/Animator.hpp"
#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/Renderables/AnimatedModel.hpp"

namespace clove {
    struct AnimatedModelComponent {
        AssetPtr<AnimatedModel> model;
        std::shared_ptr<Material> material{ std::make_shared<Material>() };
        Animator animator;
    };
}