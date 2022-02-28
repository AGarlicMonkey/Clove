#pragma once

#include "Clove/FileSystem/AssetPtr.hpp"
#include "Clove/FileSystem/VirtualFileSystem.hpp"
#include "Clove/Rendering/Renderables/AnimatedModel.hpp"
#include "Clove/Rendering/Renderables/StaticModel.hpp"
#include "Clove/Rendering/Renderables/Texture.hpp"
#include "Clove/SoundFile.hpp"

#include <Clove/Guid.hpp>
#include <unordered_map>

namespace clove::serialiser {
    class Node;
}

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

        /**
         * @brief Resets any AssetPtrs for the asset along with removing all references inside the AssetManager.
         * @param filePath 
         */
        inline void removeStaticModel(Guid const assetGuid);
        /**
         * @copydoc removeStaticModel
         */
        inline void removeAnimatedModel(Guid const assetGuid);
        /**
         * @copydoc removeStaticModel
         */
        inline void removeTexture(Guid const assetGuid);
        /**
         * @copydoc removeStaticModel
         */
        inline void removeSound(Guid const assetGuid);

        /**
         * @brief Writes the entire contents of the asset manager into the node.
         * @param node 
         */
        void serialise(serialiser::Node &node);

        /**
         * @brief Rebuilds the asset manager from a node.
         * @param node 
         */
        void deserialise(serialiser::Node &node);

    private:
        AssetPtr<StaticModel> addStaticModel(VirtualFileSystem::Path const &filePath, Guid guid);
        AssetPtr<AnimatedModel> addAnimatedModel(VirtualFileSystem::Path const &filePath, Guid guid);
        AssetPtr<Texture> addTexture(VirtualFileSystem::Path const &filePath, Guid guid);
        AssetPtr<SoundFile> addSound(VirtualFileSystem::Path const &filePath, Guid guid);

        template<typename AssetType>
        void removeAsset(std::unordered_map<std::string, AssetPtr<AssetType>> &container, Guid const assetGuid);
    };
}

#include "AssetManager.inl"