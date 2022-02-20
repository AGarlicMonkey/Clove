#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr()
        : asset{ std::make_shared<std::optional<AssetType>>() } {
    }

    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr(size_t hash, std::function<AssetType()> loadFunction)
        : assetHash{ hash }
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
        return assetHash != 0 && loadFunction != nullptr;
    }

    template<typename AssetType>
    bool AssetPtr<AssetType>::isLoaded() const {
        return asset->has_value();
    }

    template<typename AssetType>
    AssetType &AssetPtr<AssetType>::get() {
        CLOVE_ASSERT_MSG(isValid(), "{0}: AssetPtr requires a valid path before it can load", CLOVE_FUNCTION_NAME_PRETTY);

        if(!isLoaded()) {
            *asset = loadFunction();
        }

        return asset->value();
    }

    template<typename AssetType>
    AssetType const &AssetPtr<AssetType>::get() const {
        CLOVE_ASSERT_MSG(isValid(), "{0}: AssetPtr requires a valid path before it can load", CLOVE_FUNCTION_NAME_PRETTY);

        if(!isLoaded()) {
            *asset = loadFunction();
        }

        return asset->value();
    }

    template<typename AssetType>
    size_t AssetPtr<AssetType>::getHash() const {
        return assetHash;
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