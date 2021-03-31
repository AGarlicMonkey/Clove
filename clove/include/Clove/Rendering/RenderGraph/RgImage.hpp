#pragma once

#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Maths/Vector.hpp>
#include <memory>

namespace garlic::clove {
    class GhaFactory;
}

namespace garlic::clove {
    /**
     * @brief Tracks the usage and dependencies of an image in the RenderGraph.
     * @details Can be used to later create a GhaImage when executing the graph.
     */
    class RgImage {
        //VARIABLES
    private:
        GhaImage::Descriptor ghaDescriptor{}; /**< Gets updated while the image is used within the graph. */

        //FUNCTIONS
    public:
        RgImage() = delete;
        RgImage(GhaImage::Type imagetype, vec2ui dimensions);

        RgImage(RgImage const &other);
        RgImage(RgImage &&other) noexcept;

        RgImage &operator=(RgImage const &other);
        RgImage &operator=(RgImage &&other) noexcept;

        ~RgImage();

        /**
         * @brief Creates a GhaImage with it's usage/layout set based on how it's used in the graph. 
         * @param factory 
         * @return 
         */
        std::unique_ptr<GhaImage> createGhaImage(GhaFactory &factory);
    };
}