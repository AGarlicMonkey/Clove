#pragma once

#include <filesystem>
#include <optional>
#include <functional>

namespace garlic::clove {
    /**
     * @brief Points to an asset on disk. Can load the asset when required.
     * @tparam AssetType 
     */
    template<typename AssetType>
    class AssetPtr {
        //VARIABLES
    private:
        std::filesystem::path assetPath{};
        std::function<AssetType(std::filesystem::path const &)> loadFunction{};

        std::optional<AssetType> asset{};

        //FUNCTIONS
    public:
        AssetPtr();
        AssetPtr(std::filesystem::path assetPath, std::function<AssetType(std::filesystem::path const &)> loadFunction);

        AssetPtr(AssetPtr const &other) = delete;
        AssetPtr(AssetPtr &&other) noexcept;

        AssetPtr &operator=(AssetPtr const &other) = delete;
        AssetPtr &operator=(AssetPtr &&other) noexcept;

        ~AssetPtr();

        /**
         * @brief Returns true if this has a valid path to an asset.
         * @return 
         */
        bool isValid() const;

        /**
         * @brief Returns true if this has loaded the asset and there for contains a valid object.
         * @return 
         */
        bool isLoaded() const;

        /**
         * @brief Returns the asset object. Will load synchronously if the asset is not yet loaded.
         * @return 
         */
        [[nodiscard]] AssetType &get();

        /**
         * @copydoc AssetPtr::get 
         */
        [[nodiscard]] AssetType const &get() const;

        AssetType *operator->();
        AssetType const *operator->() const;

        AssetType &operator*();
        AssetType const &operator*() const;
    };
}

#include "AssetPtr.inl"