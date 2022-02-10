#pragma once

#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/Renderables/StaticModel.hpp"

#include <Clove/Reflection/Reflection.hpp>

namespace clove {
    struct StaticModelComponent {
        AssetPtr<StaticModel> model;
        Material material{};
    };
}

CLOVE_REFLECT_DECLARE_TYPE(clove::StaticModelComponent)