#pragma once

#include "Clove/Application.hpp"
#include "Clove/FileSystem/AssetManager.hpp"
#include "Clove/Rendering/Material.hpp"
#include "Clove/Rendering/Renderables/StaticModel.hpp"
#include "Clove/SerialisationCommon.hpp"

namespace clove {
    struct StaticModelComponent {
        AssetPtr<StaticModel> model;
        std::shared_ptr<Material> material{ std::make_shared<Material>() };
    };
}

namespace clove {
    template<>
    inline serialiser::Node serialise(StaticModelComponent const &object) {
        serialiser::Node node{};
        node["version"]  = 1;
        node["path"]     = object.model.getPath().string();
        //TEMP: Currently can't serialise pointers so for now a new material needs to be made
        node["material"] = *object.material;
        return node;
    }

    template<>
    inline StaticModelComponent deserialise(serialiser::Node const &node) {
        return StaticModelComponent{
            Application::get().getAssetManager()->getStaticModel(node["path"].as<std::string>()),
            std::make_shared<Material>(node["material"].as<Material>()),
        };
    }
}