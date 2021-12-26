#pragma once

#include "Clove/Rendering/RenderingConstants.hpp"

#include <Clove/Maths/Matrix.hpp>
#include <Clove/Maths/Vector.hpp>
#include <array>

namespace clove {
    struct alignas(16) MaterialData {
        float sininess;
    };

    struct ModelData {
        mat4f model;
        mat4f inverseTransposeModel;
    };

    struct ViewData {
        mat4f view;
        mat4f projection;
    };

    //Lighting data passed to GPU
    struct LightData {
        vec3f direction{ 0.0f, 0.0f, 0.0f };
        uint32_t shadowIndex{ 0 };

        vec3f position{ 0.0f, 0.0f, 0.0f };
        float constant{ 1.0f };

        vec3f ambient{ 0.01f, 0.01f, 0.01f };
        float linear{ 0.0014f };

        vec3f diffuse{ 0.75f, 0.75f, 0.75f };
        float quadratic{ 0.000007f };

        vec3f specular{ 1.0f, 1.0f, 1.0f };
        float radius{ 0 };

        int32_t type{ 0 };
        float padding_1;
        float padding_2;
        float padding_3;
    };

    struct SkeletalData {
        std::array<mat4f, MAX_JOINTS> jointTransforms;
    };
}