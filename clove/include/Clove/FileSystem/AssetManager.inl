#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveAssetManager)

namespace clove {
    AssetManager::AssetManager(VirtualFileSystem *vfs)
        : vfs{ vfs } {
    }

    AssetManager::AssetManager(AssetManager &&other) noexcept = default;

    AssetManager &AssetManager::operator=(AssetManager &&other) noexcept = default;

    AssetManager::~AssetManager() = default;

    void AssetManager::moveStaticModel(VirtualFileSystem::Path const &sourcePath, VirtualFileSystem::Path const &destPath) {
        moveAsset(staticModels, sourcePath, destPath);
    }

    void AssetManager::moveAnimatedModel(VirtualFileSystem::Path const &sourcePath, VirtualFileSystem::Path const &destPath) {
        moveAsset(animatedModels, sourcePath, destPath);
    }

    void AssetManager::moveTexture(VirtualFileSystem::Path const &sourcePath, VirtualFileSystem::Path const &destPath) {
        moveAsset(textures, sourcePath, destPath);
    }

    void AssetManager::moveSound(VirtualFileSystem::Path const &sourcePath, VirtualFileSystem::Path const &destPath) {
        moveAsset(sounds, sourcePath, destPath);
    }

    void AssetManager::removeStaticModel(Guid const assetGuid) {
        removeAsset(staticModels, assetGuid);
    }

    void AssetManager::removeAnimatedModel(Guid const assetGuid) {
        removeAsset(animatedModels, assetGuid);
    }

    void AssetManager::removeTexture(Guid const assetGuid) {
        removeAsset(textures, assetGuid);
    }

    void AssetManager::removeSound(Guid const assetGuid) {
        removeAsset(sounds, assetGuid);
    }

    template<typename AssetType>
    void AssetManager::moveAsset(std::unordered_map<std::string, AssetPtr<AssetType>> &container, VirtualFileSystem::Path const &sourcePath, VirtualFileSystem::Path const &destPath) {
        AssetPtr<AssetType> asset{ container.at(sourcePath.string()) };
        
        if(asset.isValid()) {
            container.erase(sourcePath.string());
            container[destPath.string()] = asset;

            CLOVE_LOG(CloveAssetManager, LogLevel::Trace, "Moved {0} to {1}.", sourcePath.string(), destPath.string());
        }
    }

    template<typename AssetType>
    void AssetManager::removeAsset(std::unordered_map<std::string, AssetPtr<AssetType>> &container, Guid const assetGuid) {
        AssetPtr<AssetType> asset{};
        std::string assetPath{};

        for(auto &&[path, assetPtr] : container) {
            if(assetPtr.getGuid() == assetGuid) {
                asset     = assetPtr;
                assetPath = path;
                break;
            }
        }

        if(asset.isValid()) {
            *asset.loadFunction.get() = nullptr;
            asset.asset->reset();

            container.erase(assetPath);

            CLOVE_LOG(CloveAssetManager, LogLevel::Trace, "Deleted Asset {0}.", assetPath);
        }
    }
}