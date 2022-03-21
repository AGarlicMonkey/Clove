#pragma once

#include "Clove/Graphics/GhaImage.hpp"
#include "Clove/Graphics/Result.hpp"

#include <Clove/Maths/Vector.hpp>
#include <vector>

namespace clove {
    class GhaSemaphore;
    class GhaImageView;
}

namespace clove {
    /**
     * @brief A GhaSwapchain allows presentation of rendering results to a Window
     */
    class GhaSwapchain {
        //TYPES
    public:
        struct Descriptor {
            vec2ui extent;            /**< Size of the swapchain's backing images. */
            uint32_t imageCount{ 3 }; /**< How many images the swapchain will hold. */
        };

        //FUNCTIONS
    public:
        virtual ~GhaSwapchain() = default;

        /**
         * @brief Aquire the next available image that can be rendered to.
         * @param availableSemaphore A Sempahore that the GhaSwapchain will signal when the image is ready, can be nullptr.
         * @return Returns a pair that contains an image which can be rendered to and a result for if the process was successful.
         * Note: There can be some instances where the image is nullptr, particularly if the swapchain is out of date.
         */
        virtual std::pair<GhaImage *, Result> aquireNextImage(GhaSemaphore const *availableSemaphore) = 0;

        /**
         * @brief Returns the index of the image from the last aquireNextImage call. This can be used to 
         * track resources tied to a specific swapchain image.
         * @return 
         */
        virtual uint32_t getCurrentImageIndex() const = 0;

        /**
         * @brief Presents the swapchain to the window it is attached to. The image that will be presented
         * is the one from the most recent aquireNextImage call.
         * @param waitSemaphores An array of semaphores the presentation should wait on before happening.
         * @return
         */
        virtual Result present(std::vector<GhaSemaphore const *> waitSemaphores) = 0;

        virtual GhaImage::Format getImageFormat() const = 0;
        virtual vec2ui getSize() const                  = 0;
    };
}
