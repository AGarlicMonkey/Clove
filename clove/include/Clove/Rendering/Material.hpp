#pragma once

#include "Clove/Application.hpp"
#include "Clove/FileSystem/AssetManager.hpp"
#include "Clove/FileSystem/AssetPtr.hpp"
#include "Clove/Rendering/Renderables/Texture.hpp"
#include "Clove/SerialisationCommon.hpp"

#include <Clove/Maths/Vector.hpp>
#include <memory>

namespace clove {
    class GhaFactory;
    class GhaImage;
    class GhaImageView;
}

namespace clove {
    /**
	 * @brief A Material contains values that will be passed to shaders to customise
	 * what an object looks like.
	 */
    class Material {
        //VARIABLES
    private:
        //Default images and views - fall backs for if a texture hasn't been provided.
        static std::weak_ptr<GhaImage> defaultImage;
        static std::weak_ptr<GhaImageView> defaultView;

        std::shared_ptr<GhaImage> localImage;
        std::shared_ptr<GhaImageView> localView;

        AssetPtr<Texture> diffuseTexture;
        AssetPtr<Texture> specularTexture;

        vec4f colour{ 1.0f };
        float shininess{ 32.0f };

        //FUNCTIONS
    public:
        Material();

        Material(Material const &other);
        Material(Material &&other) noexcept;

        Material &operator=(Material const &other);
        Material &operator=(Material &&other) noexcept;

        ~Material();

        inline void setDiffuseTexture(AssetPtr<Texture> texture);
        std::shared_ptr<GhaImageView> getDiffuseView() const;

        inline void setSpecularTexture(AssetPtr<Texture> texture);
        std::shared_ptr<GhaImageView> getSpecularView() const;

        inline void setColour(vec4f colour);
        inline vec4f const &getColour() const;

        inline void setShininess(float shininess);
        inline float getShininess() const;

        //TEMP - for serialisation will be removed in the future
        AssetPtr<Texture> getDiffuseTexture() const {
            return diffuseTexture;
        };
        AssetPtr<Texture> getSpecularTexture() const {
            return specularTexture;
        };
        //
    };
}

#include "Material.inl"

namespace clove {
    template<>
    inline serialiser::Node serialise(Material const &object) {
        serialiser::Node node{};
        node["version"] = 1;

        if(object.getDiffuseTexture().isValid()) {
            node["diffuse"] = object.getDiffuseTexture().getPath().string();
        } else {
            node["diffuse"] = "none";
        }

        if(object.getSpecularTexture().isValid()) {
            node["specular"] = object.getSpecularTexture().getPath().string();
        } else {
            node["specular"] = "none";
        }

        node["colour"]    = object.getColour();
        node["shininess"] = object.getShininess();

        return node;
    }

    template<>
    inline Material deserialise(serialiser::Node const &node) {
        Material material{};

        auto const diffusePath{ node["diffuse"].as<std::string>() };
        auto const specularPath{ node["specular"].as<std::string>() };

        if(diffusePath != "none") {
            material.setDiffuseTexture(Application::get().getAssetManager()->getTexture(diffusePath));
        }
        if(specularPath != "none") {
            material.setSpecularTexture(Application::get().getAssetManager()->getTexture(specularPath));
        }
        material.setColour(node["colour"].as<vec4f>());
        material.setShininess(node["shininess"].as<float>());

        return material;
    }
}
