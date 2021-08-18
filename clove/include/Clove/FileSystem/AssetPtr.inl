#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr()
        : assetPath{ std::make_shared<std::filesystem::path>() }
        , asset{ std::make_shared<std::optional<AssetType>>() } {
    }

    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr(std::filesystem::path assetVfsPath, std::function<AssetType()> loadFunction)
        : assetPath{ std::make_shared<std::filesystem::path>(std::move(assetVfsPath)) }
        , loadFunction{ std::move(loadFunction) }
        , asset{ std::make_shared<std::optional<AssetType>>() } {
    }

    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr(AssetPtr const &other) = default;

    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr(AssetPtr &&other) noexcept = default;

    template<typename AssetType>
    AssetPtr<AssetType> &AssetPtr<AssetType>::operator=(AssetPtr const &other) = default;

    template<typename AssetType>
    AssetPtr<AssetType> &AssetPtr<AssetType>::operator=(AssetPtr &&other) noexcept = default;

    template<typename AssetType>
    AssetPtr<AssetType>::~AssetPtr() = default;

    template<typename AssetType>
    bool AssetPtr<AssetType>::isValid() const {
        return !assetPath->empty() && loadFunction != nullptr;
    }

    template<typename AssetType>
    bool AssetPtr<AssetType>::isLoaded() const {
        return asset->has_value();
    }

    template<typename AssetType>
    AssetType &AssetPtr<AssetType>::get() {
        CLOVE_ASSERT(isValid(), "{0}: AssetPtr requires a valid path before it can load");

        if(!isLoaded()) {
            *asset = loadFunction();
        }

        return asset->value();
    }

    template<typename AssetType>
    AssetType const &AssetPtr<AssetType>::get() const {
        CLOVE_ASSERT(isValid(), "{0}: AssetPtr requires a valid path before it can load");

        if(!isLoaded()) {
            *asset = loadFunction();
        }

        return asset->value();
    }

    template<typename AssetType>
    std::filesystem::path const &AssetPtr<AssetType>::getPath() const {
        return *assetPath;
    }

    template<typename AssetType>
    AssetType *AssetPtr<AssetType>::operator->() {
        return &get();
    }

    template<typename AssetType>
    AssetType const *AssetPtr<AssetType>::operator->() const {
        return &get();
    }

    template<typename AssetType>
    AssetType &AssetPtr<AssetType>::operator*() {
        return get();
    }

    template<typename AssetType>
    AssetType const &AssetPtr<AssetType>::operator*() const {
        return get();
    }
}