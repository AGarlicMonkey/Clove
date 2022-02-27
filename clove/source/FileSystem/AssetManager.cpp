#include "Clove/FileSystem/AssetManager.hpp"

#include "Clove/FileSystem/VirtualFileSystem.hpp"
#include "Clove/ModelLoader.hpp"
#include "Clove/TextureLoader.hpp"

#include <Clove/Log/Log.hpp>
#include <Clove/Serialisation/Node.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveAssetManager)

namespace clove {
    AssetPtr<StaticModel> AssetManager::getStaticModel(VirtualFileSystem::Path const &filePath) {
        AssetPtr<StaticModel> &model{ staticModels[filePath.string()] };
        if(!model.isValid()) {
            model = addStaticModel(filePath, Guid{});
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
        AssetPtr<AnimatedModel> &model{ animatedModels[filePath.string()] };
        if(!model.isValid()) {
            model = addAnimatedModel(filePath, Guid{});
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
        AssetPtr<Texture> &texture{ textures[filePath.string()] };
        if(!texture.isValid()) {
            texture = addTexture(filePath, Guid{});
        }

        return texture;
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
        AssetPtr<SoundFile> &sound{ sounds[filePath.string()] };
        if(!sound.isValid()) {
            sound = addSound(filePath, Guid{});
        }

        return sound;
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

    void AssetManager::serialise(serialiser::Node &node) {
        node["file_version"] = 1;

        for(auto &&[path, ptr] : staticModels) {
            node["static_models"][path] = static_cast<Guid::Type>(ptr.getGuid());
        }
        for(auto &&[path, ptr] : animatedModels) {
            node["animated_models"][path] = static_cast<Guid::Type>(ptr.getGuid());
        }
        for(auto &&[path, ptr] : textures) {
            node["textures"][path] = static_cast<Guid::Type>(ptr.getGuid());
        }
        for(auto &&[path, ptr] : sounds) {
            node["sounds"][path] = static_cast<Guid::Type>(ptr.getGuid());
        }
    }

    void AssetManager::deserialise(serialiser::Node &node) {
        for(auto &node : node["static_models"]) {
            std::string const path{ node.getKey() };
            Guid const guid{ node.as<Guid::Type>() };

            staticModels[path] = addStaticModel(path, guid);
        }
        for(auto &node : node["animated_models"]) {
            std::string const path{ node.getKey() };
            Guid const guid{ node.as<Guid::Type>() };

            animatedModels[path] = addAnimatedModel(path, guid);
        }
        for(auto &node : node["textures"]) {
            std::string const path{ node.getKey() };
            Guid const guid{ node.as<Guid::Type>() };

            textures[path] = addTexture(path, guid);
        }
        for(auto &node : node["sounds"]) {
            std::string const path{ node.getKey() };
            Guid const guid{ node.as<Guid::Type>() };

            sounds[path] = addSound(path, guid);
        }
    }

    AssetPtr<StaticModel> AssetManager::addStaticModel(VirtualFileSystem::Path const &filePath, Guid guid) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        return AssetPtr<StaticModel>{ guid, [fullSystemPath]() {
                                         return ModelLoader::loadStaticModel(fullSystemPath);
                                     } };
    }

    AssetPtr<AnimatedModel> AssetManager::addAnimatedModel(VirtualFileSystem::Path const &filePath, Guid guid) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        return AssetPtr<AnimatedModel>{ guid, [fullSystemPath]() {
                                           return ModelLoader::loadAnimatedModel(fullSystemPath);
                                       } };
    }

    AssetPtr<Texture> AssetManager::addTexture(VirtualFileSystem::Path const &filePath, Guid guid) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        return AssetPtr<Texture>{ guid, [fullSystemPath]() {
                                     return TextureLoader::loadTexture(fullSystemPath).getValue();
                                 } };
    }

    AssetPtr<SoundFile> AssetManager::addSound(VirtualFileSystem::Path const &filePath, Guid guid) {
        std::filesystem::path const fullSystemPath{ vfs->resolve(filePath) };
        if(!std::filesystem::exists(fullSystemPath)) {
            CLOVE_LOG(CloveAssetManager, LogLevel::Error, "{0} does not resolve to a file that exists on disk.", filePath.string());
            return {};
        }

        return AssetPtr<SoundFile>{ guid, [fullSystemPath]() {
                                       return SoundFile{ fullSystemPath.string() };
                                   } };
    }
}