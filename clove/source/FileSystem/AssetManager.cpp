#include "Clove/FileSystem/AssetManager.hpp"

#include "Clove/FileSystem/VirtualFileSystem.hpp"
#include "Clove/ModelLoader.hpp"
#include "Clove/TextureLoader.hpp"

#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveAssetManager)

namespace clove {
    AssetPtr<StaticModel> AssetManager::getStaticModel(VirtualFileSystem::Path const &filePath) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        AssetPtr<StaticModel> &model{ staticModels[filePath.string()] };
        if(!model.isValid()) {
            model = AssetPtr<StaticModel>{ [fullSystemPath]() {
                return ModelLoader::loadStaticModel(fullSystemPath);
            } };
        }

        return model;
    }

    AssetPtr<StaticModel> AssetManager::getStaticModel(Guid const assetGuid) {
        AssetPtr<StaticModel> asset{};

        for(auto &&[path, assetPtr] : staticModels) {
            if(assetPtr.getGuid() == assetGuid) {
                asset = assetPtr;
                break;
            }
        }

        if(!asset.isValid()) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find StaticModel with guid {0}.", assetGuid);
        }

        return asset;
    }

    AssetPtr<AnimatedModel> AssetManager::getAnimatedModel(VirtualFileSystem::Path const &filePath) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        AssetPtr<AnimatedModel> &model{ animatedModels[filePath.string()] };
        if(!model.isValid()) {
            model = AssetPtr<AnimatedModel>{ [fullSystemPath]() {
                return ModelLoader::loadAnimatedModel(fullSystemPath);
            } };
        }

        return model;
    }

    AssetPtr<AnimatedModel> AssetManager::getAnimatedModel(Guid const assetGuid) {
        AssetPtr<AnimatedModel> asset{};

        for(auto &&[path, assetPtr] : animatedModels) {
            if(assetPtr.getGuid() == assetGuid) {
                asset = assetPtr;
                break;
            }
        }

        if(!asset.isValid()) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find AnimatedModel with guid {0}.", assetGuid);
        }

        return asset;
    }

    AssetPtr<Texture> AssetManager::getTexture(VirtualFileSystem::Path const &filePath) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        AssetPtr<Texture> &model{ textures[filePath.string()] };
        if(!model.isValid()) {
            model = AssetPtr<Texture>{ [fullSystemPath]() {
                return TextureLoader::loadTexture(fullSystemPath).getValue();
            } };
        }

        return model;
    }

    AssetPtr<Texture> AssetManager::getTexture(Guid const assetGuid) {
        AssetPtr<Texture> asset{};

        for(auto &&[path, assetPtr] : textures) {
            if(assetPtr.getGuid() == assetGuid) {
                asset = assetPtr;
                break;
            }
        }

        if(!asset.isValid()) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find Textures with guid {0}.", assetGuid);
        }

        return asset;
    }

    AssetPtr<SoundFile> AssetManager::getSound(VirtualFileSystem::Path const &filePath) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        AssetPtr<SoundFile> &model{ sounds[filePath.string()] };
        if(!model.isValid()) {
            model = AssetPtr<SoundFile>{ [fullSystemPath]() {
                return SoundFile{ fullSystemPath.string() };
            } };
        }

        return model;
    }

    AssetPtr<SoundFile> AssetManager::getSound(Guid const assetGuid) {
        AssetPtr<SoundFile> asset{};

        for(auto &&[path, assetPtr] : sounds) {
            if(assetPtr.getGuid() == assetGuid) {
                asset = assetPtr;
                break;
            }
        }

        if(!asset.isValid()) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find SoundFile with guid {0}.", assetGuid);
        }

        return asset;
    }
}