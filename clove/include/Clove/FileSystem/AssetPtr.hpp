#pragma once

#include <filesystem>
#include <optional>
#include <functional>
#include <memory>

namespace clove {
    /**
     * @brief Points to an asset on disk. Can load the asset when required.
     * @tparam AssetType 
     */
    template<typename AssetType>
    class AssetPtr {
        //VARIABLES
    private:
        std::shared_ptr<std::filesystem::path> assetPath{};
        std::function<AssetType()> loadFunction{};

        std::shared_ptr<std::optional<AssetType>> asset{};

        //FUNCTIONS
    public:
        AssetPtr();
        AssetPtr(std::filesystem::path assetVfsPath, std::function<AssetType()> loadFunction);

        AssetPtr(AssetPtr const &other);
        AssetPtr(AssetPtr &&other) noexcept;

        AssetPtr &operator=(AssetPtr const &other);
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

        /**
         * @brief Returns the path of the asset.
         * @details Path will be in the VirtualFileSystem format.
         * @return 
         */
        [[nodiscard]] std::filesystem::path const &getPath() const;

        AssetType *operator->();
        AssetType const *operator->() const;

        AssetType &operator*();
        AssetType const &operator*() const;
    };
}

#include "AssetPtr.inl"