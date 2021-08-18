#pragma once

#include "Clove/SerialisationCommon.hpp"

#include <Clove/Maths/MathsHelpers.hpp>
#include <Clove/Maths/Matrix.hpp>
#include <Clove/Maths/Vector.hpp>

namespace clove {
    struct PointLightComponent {
        vec3f ambientColour{ 0.01f, 0.01f, 0.01f };
        vec3f diffuseColour{ 0.75f, 0.75f, 0.75f };
        vec3f specularColour{ 1.0f, 1.0f, 1.0f };
    };
}

namespace clove {
    template<>
    inline serialiser::Node serialise(PointLightComponent const &object) {
        serialiser::Node node{};
        node["ambientColour"]  = object.ambientColour;
        node["diffuseColour"]  = object.diffuseColour;
        node["specularColour"] = object.specularColour;
        return node;
    }

    template<>
    inline PointLightComponent deserialise(serialiser::Node const &node) {
        PointLightComponent component{};
        component.ambientColour  = node["ambientColour"].as<vec3f>();
        component.diffuseColour  = node["diffuseColour"].as<vec3f>();
        component.specularColour = node["specularColour"].as<vec3f>();
        return component;
    }
}