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
        //VARIABLES
    private:
        Guid guid{};
        std::shared_ptr<std::optional<AssetType>> asset{ nullptr };

        std::function<AssetType()> loadFunction{};

        //FUNCTIONS
    public:
        AssetPtr();
        AssetPtr(std::function<AssetType()> loadFunction);

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
         * @brief Returns the hash used to identify this pointer's asset.
         * @return 
         */
        [[nodiscard]] Guid getGuid() const;

        AssetType *operator->();
        AssetType const *operator->() const;

        AssetType &operator*();
        AssetType const &operator*() const;
    };
}

#include "AssetPtr.inl"