#pragma once

#include "Clove/FileSystem/AssetPtr.hpp"
#include "Clove/FileSystem/VirtualFileSystem.hpp"
#include "Clove/Rendering/Renderables/AnimatedModel.hpp"
#include "Clove/Rendering/Renderables/StaticModel.hpp"
#include "Clove/Rendering/Renderables/Texture.hpp"
#include "Clove/SoundFile.hpp"

#include <Clove/Guid.hpp>
#include <unordered_map>

namespace clove {
    /**
     * @brief Manages the loading of assets.
     */
    class AssetManager {
        //VARIABLES
    private:
        VirtualFileSystem *vfs{ nullptr };

        std::unordered_map<std::string, AssetPtr<StaticModel>> staticModels{};
        std::unordered_map<std::string, AssetPtr<AnimatedModel>> animatedModels{};

        std::unordered_map<std::string, AssetPtr<Texture>> textures{};

        std::unordered_map<std::string, AssetPtr<SoundFile>> sounds{};

        //FUNCTIONS
    public:
        AssetManager() = delete;
        inline AssetManager(VirtualFileSystem *vfs);

        AssetManager(AssetManager const &other) = delete;
        inline AssetManager(AssetManager &&other) noexcept;

        AssetManager &operator=(AssetManager const &other) = delete;
        inline AssetManager &operator=(AssetManager &&other) noexcept;

        inline ~AssetManager();

        /**
         * @brief Get a StaticModel using a VFS path.
         * @param filePath
         * @return An AssetPtr to the StaticModel
         */
        AssetPtr<StaticModel> getStaticModel(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get a StaticModel using a guid.
         * @param assetGuid 
         * @return 
         */
        AssetPtr<StaticModel> getStaticModel(Guid const assetGuid);

        /**
         * @brief Get an AnimatedModel using a VFS path.
         * @param filePath
         * @return An AssetPtr to the AnimatedModel
         */
        AssetPtr<AnimatedModel> getAnimatedModel(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get a AnimatedModel using a guid.
         * @param assetGuid 
         * @return 
         */
        AssetPtr<AnimatedModel> getAnimatedModel(Guid const assetGuid);

        /**
         * @brief Get a Texture using a VFS guid.
         * @param filePath
         * @return An AssetPtr to the Texture
         */
        AssetPtr<Texture> getTexture(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get a Texture using a hash.
         * @param assetGuid 
         * @return 
         */
        AssetPtr<Texture> getTexture(Guid const assetGuid);

        /**
         * @brief Get a SoundFile using a VFS path.
         * @param filePath
         * @return An AssetPtr to the SoundFile
         */
        AssetPtr<SoundFile> getSound(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get a SoundFile using a guid.
         * @param assetGuid 
         * @return 
         */
        AssetPtr<SoundFile> getSound(Guid const assetGuid);
    };
}

#include "AssetManager.inl"