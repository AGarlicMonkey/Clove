#pragma once

#include "Clove/Application.hpp"
#include "Clove/FileSystem/AssetManager.hpp"
#include "Clove/FileSystem/AssetPtr.hpp"
#include "Clove/Rendering/Renderables/Texture.hpp"

#include <Clove/Reflection/Reflection.hpp>
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
        CLOVE_REFLECT_PRIVATE(Material)
        
        //VARIABLES
    private:
        //Default image - fall back for if a texture hasn't been provided.
        static std::weak_ptr<GhaImage> defaultImage;

        std::shared_ptr<GhaImage> localDefaultImage;

        AssetPtr<Texture> diffuseTexture;
        AssetPtr<Texture> specularTexture;

        vec4f colour{ 1.0f };
        float shininess{ 32.0f };//TODO: Not hooked up to the shaders

        //FUNCTIONS
    public:
        Material();

        Material(Material const &other);
        Material(Material &&other) noexcept;

        Material &operator=(Material const &other);
        Material &operator=(Material &&other) noexcept;

        ~Material();

        inline void setDiffuseTexture(AssetPtr<Texture> texture);
        GhaImage *getDiffuseImage() const;

        inline void setSpecularTexture(AssetPtr<Texture> texture);
        GhaImage *getSpecularImage() const;

        inline void setColour(vec4f colour);
        inline vec4f const &getColour() const;

        inline void setShininess(float shininess);
        inline float getShininess() const;
    };
}

CLOVE_REFLECT_DECLARE_TYPE(clove::Material)

#include "Material.inl"