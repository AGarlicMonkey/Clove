#pragma once

#include <Clove/Delegate/MultiCastDelegate.hpp>
#include <Clove/Expected.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Graphics/GhaImageView.hpp>

namespace clove {
    class GhaSemaphore;
    class GhaImage;
}

namespace clove {
    /**
     * @brief Contains an array of 1-N images that can be rendered to.
     */
    class RenderTarget {
        //VARIABLES
    public:
        /**
         * @brief Gets called when properties of this render target are about to change. 
         * Allowing objects referencing any (i.e ImageViews) to be reset ahead of time.
         */
        MultiCastDelegate<void()> onPropertiesChangedBegin{};
        /**
         * @brief Gets called once properties of the render target have finished changing.
         * Allowing objects referencing those properties to be created
         */
        MultiCastDelegate<void()> onPropertiesChangedEnd{};

        //FUNCTIONS
    public:
        RenderTarget() = default;

        RenderTarget(RenderTarget const &other)     = delete;
        RenderTarget(RenderTarget &&other) noexcept = default;

        RenderTarget &operator=(RenderTarget const &other) = delete;
        RenderTarget &operator=(RenderTarget &&other) noexcept = default;

        virtual ~RenderTarget() = default;

        /**
         * @brief Aquire the next available image that can be rendered to.
         * @param signalSemaphore A semaphore this RenderTarget will signal when the image is ready to render to. Can be nullptr.
         * @return Returns the image index for the getImageViews array.
         */
        virtual Expected<GhaImage *, std::string> aquireNextImage(GhaSemaphore const *const signalSemaphore) = 0;

        virtual uint32_t getCurrentImageIndex() const = 0;

        /**
         * @brief Presents the render target with imageIndex.
         * @param imageIndex The image index of the getImageViews array this submission is for.
         * @param waitSemaphores Semaphores the RenderTarget will wait on before beginning presentation logic.
         */
        virtual void present(std::vector<GhaSemaphore const *> waitSemaphores) = 0;

        virtual GhaImage::Format getImageFormat() const = 0;
        virtual vec2ui getSize() const                  = 0;

        /**
         * @brief Returns the amount of images this render target is backed by.
         * @return 
         */
        virtual uint32_t getImageCount() const = 0;
    };
}