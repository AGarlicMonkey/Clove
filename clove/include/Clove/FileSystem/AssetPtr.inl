#include <Clove/Log/Log.hpp>

namespace clove {
    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr() = default;

    template<typename AssetType>
    AssetPtr<AssetType>::AssetPtr(Guid guid, std::function<AssetType()> loadFunction)
        : guid{ guid }
        , asset{ std::make_shared<std::optional<AssetType>>() }
        , loadFunction{ std::make_shared<std::function<AssetType()>>(std::move(loadFunction)) } {
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
    bool AssetPtr<AssetType>::isValid() const noexcept {
        return asset != nullptr && loadFunction != nullptr && (*loadFunction) != nullptr;
    }

    template<typename AssetType>
    bool AssetPtr<AssetType>::isLoaded() const noexcept {
        return isValid() && asset->has_value();
    }

    template<typename AssetType>
    AssetType &AssetPtr<AssetType>::get() {
        CLOVE_ASSERT_MSG(isValid(), "{0}: AssetPtr is not valid", CLOVE_FUNCTION_NAME_PRETTY);

        if(!isLoaded()) {
            *asset = (*loadFunction)();
        }

        return asset->value();
    }

    template<typename AssetType>
    AssetType const &AssetPtr<AssetType>::get() const {
        CLOVE_ASSERT_MSG(isValid(), "{0}: AssetPtr is not valid", CLOVE_FUNCTION_NAME_PRETTY);

        if(!isLoaded()) {
            *asset = (*loadFunction)();
        }

        return asset->value();
    }

    template<typename AssetType>
    Guid AssetPtr<AssetType>::getGuid() const noexcept {
        return guid;
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