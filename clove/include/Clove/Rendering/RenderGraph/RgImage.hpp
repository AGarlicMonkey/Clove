#pragma once

#include "Clove/Rendering/RenderGraph/RgResource.hpp"

#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Graphics/GhaImageView.hpp>

namespace clove {
    class RgFrameCache;
}

namespace clove {
    class RgImage : public RgResource {
        //VARIABLES
    private:
        GhaImage *ghaImage{ nullptr }; /**< The GHA image this object views. */
        GhaImage::Descriptor ghaImageDescriptor{};
        bool externalImage{ false }; /**< Will be true if this was registered with an external image. Preventing the descriptor being built. */

        //FUNCTIONS
    public:
        RgImage() = delete;
        RgImage(GhaImage::Type const imageType, GhaImage::Format const format, vec2ui const dimensions, GhaImage::Layout const initialLayout, uint32_t const arrayCount);
        RgImage(GhaImage *ghaImage);

        RgImage(RgImage const &other) = delete;
        RgImage(RgImage &&other) noexcept;

        RgImage &operator=(RgImage const &other) = delete;
        RgImage &operator=(RgImage &&other) noexcept;

        ~RgImage();

        /**
         * @brief Gets the underlying GHA image. Creates a new one if neccessary.
         * @param cache
         * @return 
         */
        GhaImage *getGhaImage(RgFrameCache &cache);
        /**
         * @brief Creates an image view for this object's internal image.
         * @param cache 
         * @param arrayIndex 
         * @param arrayCount 
         * @return 
         */
        GhaImageView *getGhaImageView(RgFrameCache &cache, uint32_t const arrayIndex, uint32_t const arrayCount);

        inline GhaImage::Format getFormat() const;
        inline vec2ui const &getDimensions() const;
        inline GhaImage::Layout getInitialLayout() const;

        inline bool isExternalImage() const;

        void addImageUsage(GhaImage::UsageMode const usage);
    };
}

#include "RgImage.inl"