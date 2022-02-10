#pragma once

#include "Clove/Rendering/Camera.hpp"

#include <Clove/Reflection/Reflection.hpp>

namespace clove {
    struct CameraComponent {
        Camera camera;
        bool isPriority{ false }; /**< If set to true RenderSubSystem will prioritise this camera as the active camera.*/
    };
}

CLOVE_REFLECT_DECLARE_TYPE(clove::CameraComponent)