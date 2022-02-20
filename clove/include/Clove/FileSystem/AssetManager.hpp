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
        //TYPES
    private:
        template<typename AssetType>
        struct AssetEntry {
            VirtualFileSystem::Path path{};
            AssetPtr<AssetType> asset{};
        };

        //VARIABLES
    private:
        VirtualFileSystem *vfs{ nullptr };

        std::unordered_map<size_t, AssetEntry<StaticModel>> staticModels{};
        std::unordered_map<size_t, AssetEntry<AnimatedModel>> animatedModels{};

        std::unordered_map<size_t, AssetEntry<Texture>> textures{};

        std::unordered_map<size_t, AssetEntry<SoundFile>> sounds{};

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
         * @brief Get a StaticModel using a hash.
         * @param assetHash 
         * @return 
         */
        AssetPtr<StaticModel> getStaticModel(size_t const assetHash);

        /**
         * @brief Get an AnimatedModel using a VFS path.
         * @param filePath
         * @return An AssetPtr to the AnimatedModel
         */
        AssetPtr<AnimatedModel> getAnimatedModel(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get a AnimatedModel using a hash.
         * @param assetHash 
         * @return 
         */
        AssetPtr<AnimatedModel> getAnimatedModel(size_t const assetHash);

        /**
         * @brief Get a Texture using a VFS path.
         * @param filePath
         * @return An AssetPtr to the Texture
         */
        AssetPtr<Texture> getTexture(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get a Texture using a hash.
         * @param assetHash 
         * @return 
         */
        AssetPtr<Texture> getTexture(size_t const assetHash);

        /**
         * @brief Get a SoundFile using a VFS path.
         * @param filePath
         * @return An AssetPtr to the SoundFile
         */
        AssetPtr<SoundFile> getSound(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get a SoundFile using a hash.
         * @param assetHash 
         * @return 
         */
        AssetPtr<SoundFile> getSound(size_t const assetHash);
    };
}

#include "AssetManager.inl"