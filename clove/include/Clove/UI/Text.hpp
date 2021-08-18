#pragma once

#include "Clove/Rendering/Renderables/Font.hpp"
#include "Clove/UI/DrawableElement.hpp"

#include <vector>

namespace clove {
    class GrpahicsFactory;
}

namespace clove {
    class Text : public DrawableElement {
        //VARIABLES
    private:
        std::shared_ptr<GhaFactory> graphicsFactory;

        Font font;
        std::string text;

        bool isBufferDirty{ false };
        std::vector<Font::Glyph> characters;

        vec2f position{ 0.0f, 0.0f };
        float rotation{ 0.0f };
        vec2f textScale{ 1.0f, 1.0f };

        //FUNCTIONS
    public:
        Text() = delete;
        Text(Font font);

        Text(Text const &other);
        Text(Text &&other) noexcept;

        Text &operator=(Text const &other);
        Text &operator=(Text &&other) noexcept;

        ~Text();

        inline void setPosition(vec2f position);
        inline void setRotation(float rotation);
        inline void setScale(vec2f scale);

        inline vec2f const &getPosition() const;
        inline float getRotation() const;
        inline vec2f const &getScale() const;

        void draw(vec2f const &drawSpace) override;

        inline void setText(std::string text);
        inline void setFontSize(uint32_t const size);

        inline std::size_t getTextLength() const;
        Font::Glyph const &getBufferForCharAt(size_t index);

    private:
        void buildGlyphs();
    };
}

#include "Text.inl"