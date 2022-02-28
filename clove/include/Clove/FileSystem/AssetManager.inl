namespace clove {
    AssetManager::AssetManager(VirtualFileSystem *vfs)
        : vfs{ vfs } {
    }

    AssetManager::AssetManager(AssetManager &&other) noexcept = default;

    AssetManager &AssetManager::operator=(AssetManager &&other) noexcept = default;

    AssetManager::~AssetManager() = default;

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
        }
    }
}