#pragma once

namespace clove {
    /**
     * @brief Describes how a shader should sample a GhaImage.
     */
    class GhaSampler {
        //TYPES
    public:
        enum class Filter {
            Nearest,
            Linear
        };
        enum class AddressMode {
            Repeat,
            MirroredRepeat,
            ClampToEdge, /**< Stretches the image from it's edge to the edge of the target */
            ClampToBorder
        };

        struct Descriptor {
            Filter minFilter;
            Filter magFilter;
            AddressMode addressModeU;
            AddressMode addressModeV;
            AddressMode addressModeW;
            bool enableAnisotropy{ false };
            float maxAnisotropy{ 1.0f };
        };

        //FUNCTIONS
    public:
        virtual ~GhaSampler() = default;
    };
}