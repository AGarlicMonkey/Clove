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

        size_t const pathHash{ std::hash<std::string>{}(filePath.string()) };

        AssetEntry<StaticModel> &model{ staticModels[pathHash] };
        if(!model.asset.isValid()) {
            model.path  = filePath;
            model.asset = AssetPtr<StaticModel>{ pathHash, [fullSystemPath]() {
                                                    return ModelLoader::loadStaticModel(fullSystemPath);
                                                } };
        }

        return model.asset;
    }

    AssetPtr<StaticModel> AssetManager::getStaticModel(size_t const assetHash) {
        AssetPtr<StaticModel> asset{};

        if(staticModels.contains(assetHash)) {
            asset = staticModels.at(assetHash).asset;
        }else{
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find StaticModel with hash {0}.", assetHash);
        }

        return asset;
    }

    AssetPtr<AnimatedModel> AssetManager::getAnimatedModel(VirtualFileSystem::Path const &filePath) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        size_t const pathHash{ std::hash<std::string>{}(filePath.string()) };

        AssetEntry<AnimatedModel> &model{ animatedModels[pathHash] };
        if(!model.asset.isValid()) {
            model.path  = filePath;
            model.asset = AssetPtr<AnimatedModel>{ pathHash, [fullSystemPath]() {
                                                      return ModelLoader::loadAnimatedModel(fullSystemPath);
                                                  } };
        }

        return model.asset;
    }

    AssetPtr<AnimatedModel> AssetManager::getAnimatedModel(size_t const assetHash) {
        AssetPtr<AnimatedModel> asset{};

        if(animatedModels.contains(assetHash)) {
            asset = animatedModels.at(assetHash).asset;
        } else {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find AnimatedModel with hash {0}.", assetHash);
        }

        return asset;
    }

    AssetPtr<Texture> AssetManager::getTexture(VirtualFileSystem::Path const &filePath) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        size_t const pathHash{ std::hash<std::string>{}(filePath.string()) };

        AssetEntry<Texture> &model{ textures[pathHash] };
        if(!model.asset.isValid()) {
            model.path  = filePath;
            model.asset = AssetPtr<Texture>{ pathHash, [fullSystemPath]() {
                                                return TextureLoader::loadTexture(fullSystemPath).getValue();
                                            } };
        }

        return model.asset;
    }

    AssetPtr<Texture> AssetManager::getTexture(size_t const assetHash) {
        AssetPtr<Texture> asset{};

        if(textures.contains(assetHash)) {
            asset = textures.at(assetHash).asset;
        } else {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find Texture with hash {0}.", assetHash);
        }

        return asset;
    }

    AssetPtr<SoundFile> AssetManager::getSound(VirtualFileSystem::Path const &filePath) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        size_t const pathHash{ std::hash<std::string>{}(filePath.string()) };

        AssetEntry<SoundFile> &model{ sounds[pathHash] };
        if(!model.asset.isValid()) {
            model.path  = filePath;
            model.asset = AssetPtr<SoundFile>{ pathHash, [fullSystemPath]() {
                                                  return SoundFile{ fullSystemPath.string() };
                                              } };
        }

        return model.asset;
    }

    AssetPtr<SoundFile> AssetManager::getSound(size_t const assetHash) {
        AssetPtr<SoundFile> asset{};

        if(sounds.contains(assetHash)) {
            asset = sounds.at(assetHash).asset;
        } else {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "Could not find SoundFile with hash {0}.", assetHash);
        }

        return asset;
    }
}