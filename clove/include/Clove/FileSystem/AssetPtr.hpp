#pragma once

#include <Clove/Guid.hpp>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>

namespace clove {
    /**
     * @brief Points to an asset on disk. Can load the asset when required.
     * @tparam AssetType 
     */
    template<typename AssetType>
    class AssetPtr {
        friend class AssetManager;

        //VARIABLES
    private:
        Guid guid{ 0 };

        std::shared_ptr<std::optional<AssetType>> asset{ nullptr };
        std::shared_ptr<std::function<AssetType()>> loadFunction{ nullptr };

        //FUNCTIONS
    public:
        AssetPtr();
        AssetPtr(Guid guid, std::function<AssetType()> loadFunction);

        AssetPtr(AssetPtr const &other);
        AssetPtr(AssetPtr &&other) noexcept;

        AssetPtr &operator=(AssetPtr const &other);
        AssetPtr &operator=(AssetPtr &&other) noexcept;

        ~AssetPtr();

        /**
         * @brief Returns true if this has a valid path to an asset.
         * @return 
         */
        bool isValid() const noexcept;

        /**
         * @brief Returns true if this has loaded the asset and there for contains a valid object.
         * @return 
         */
        bool isLoaded() const noexcept;

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
         * @brief Returns the hash used to identify this pointer's asset.
         * @return 
         */
        Guid getGuid() const noexcept;

        AssetType *operator->();
        AssetType const *operator->() const;

        AssetType &operator*();
        AssetType const &operator*() const;
    };
}

#include "AssetPtr.inl"