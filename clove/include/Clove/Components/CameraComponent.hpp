#pragma once

#include "Clove/Rendering/Camera.hpp"

#include <Clove/Reflection/Reflection.hpp>

namespace clove {
    struct CameraComponent {
        Camera camera;
    };
}

CLOVE_REFLECT_DECLARE_TYPE(clove::CameraComponent)