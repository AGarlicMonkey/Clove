#pragma once

#include "Clove/UI/DrawableElement.hpp"

#include <memory>
#include <Clove/Maths/Vector.hpp>

namespace clove {
    class GhaFactory;
    class GhaImage;
    class GhaImageView;
}

namespace clove {
    class Image : public DrawableElement {
        //VARIABLES
    private:
        std::shared_ptr<GhaImage> image;
        std::shared_ptr<GhaImageView> imageView;

        vec2f position{ 0.0f, 0.0f };
        float rotation{ 0.0f };
        vec2f size{ 1.0f, 1.0f };

        //FUNCTIONS
    public:
        Image();
        Image(std::shared_ptr<GhaImage> graphicsImage);

        Image(Image const &other);
        Image(Image &&other) noexcept;

        Image &operator=(Image const &other);
        Image &operator=(Image &&other) noexcept;

        ~Image();

        inline void setPosition(vec2f position);
        inline void setRotation(float rotation);
        inline void setSize(vec2f size);

        inline vec2f const &getPosition() const;
        inline float getRotation() const;
        inline vec2f const &getSize() const;

        void draw(vec2f const &drawSpace) override;
    };
}

#include "Image.inl"