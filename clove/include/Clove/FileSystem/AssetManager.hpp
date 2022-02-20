#pragma once

#include "Clove/FileSystem/AssetPtr.hpp"
#include "Clove/Rendering/Renderables/AnimatedModel.hpp"
#include "Clove/Rendering/Renderables/StaticModel.hpp"
#include "Clove/Rendering/Renderables/Texture.hpp"
#include "Clove/SoundFile.hpp"
#include "Clove/FileSystem/VirtualFileSystem.hpp"

#include <filesystem>
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
        AssetManager(VirtualFileSystem *vfs);

        AssetManager(AssetManager const &other) = delete;
        AssetManager(AssetManager &&other) noexcept;

        AssetManager &operator=(AssetManager const &other) = delete;
        AssetManager &operator=(AssetManager &&other) noexcept;

        ~AssetManager();

        /**
         * @brief Get a StaticModel
         * @param filePath
         * @return An AssetPtr to the StaticModel
         */
        AssetPtr<StaticModel> getStaticModel(VirtualFileSystem::Path const &filePath);
        /**
         * @brief Get an AnimatedModel
         * @param filePath
         * @return An AssetPtr to the AnimatedModel
         */
        AssetPtr<AnimatedModel> getAnimatedModel(VirtualFileSystem::Path const &filePath);

        /**
         * @brief Get a Texture
         * @param filePath
         * @return An AssetPtr to the Texture
         */
        AssetPtr<Texture> getTexture(VirtualFileSystem::Path const &filePath);

        /**
         * @brief Get a SoundFile
         * @param filePath
         * @return An AssetPtr to the SoundFile
         */
        AssetPtr<SoundFile> getSound(VirtualFileSystem::Path const &filePath);
    };
}