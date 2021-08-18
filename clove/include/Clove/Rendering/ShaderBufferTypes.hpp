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
    struct DirectionalLightData {
        alignas(16) vec3f direction{ 0.0f, 0.0f, 0.0f };

        alignas(16) vec3f ambient{ 0.01f, 0.01f, 0.01f };
        alignas(16) vec3f diffuse{ 0.75f, 0.75f, 0.75f };
        alignas(16) vec3f specular{ 1.0f, 1.0f, 1.0f };
    };
    struct PointLightData {
        vec3f position{ 0.0f, 0.0f, 0.0f };

        float constant{ 1.0f };
        vec3f ambient{ 0.01f, 0.01f, 0.01f };
        float linear{ 0.0014f };
        vec3f diffuse{ 0.75f, 0.75f, 0.75f };
        float quadratic{ 0.000007f };
        vec3f specular{ 1.0f, 1.0f, 1.0f };

        float farPlane{ 0 };
    };
    struct LightDataArray {
        std::array<DirectionalLightData, MAX_LIGHTS> directionalLights{};
        std::array<PointLightData, MAX_LIGHTS> pointLights{};
    };

    struct LightCount {
        uint32_t numDirectional{ 0 };
        uint32_t numPoint{ 0 };
    };

    //Lighting data needed shadows
    struct DirectionalShadowTransformArray {
        std::array<mat4f, MAX_LIGHTS> transforms;
        mat4f &operator[](const size_t index) {
            return transforms[index];
        }
    };

    //For renderer - TODO Move into that header
    struct DirectionalLight {
        DirectionalLightData data{};
        mat4f shadowTransform{};
    };
    struct PointLight {
        PointLightData data{};
        std::array<mat4f, 6> shadowTransforms{};
    };

    struct SkeletalData {
        std::array<mat4f, MAX_JOINTS> jointTransforms;
    };
}