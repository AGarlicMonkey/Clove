#pragma once

#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/Renderables/AnimatedModel.hpp"

namespace clove {
    struct AnimatedModelComponent {
        AnimatedModel model;
        std::shared_ptr<Material> material{ std::make_shared<Material>() };
    };
}