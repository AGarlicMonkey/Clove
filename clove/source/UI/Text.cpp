#include "Clove/UI/Text.hpp"

#include "Clove/Application.hpp"
#include "Clove/Rendering/Renderer.hpp"

#include <Clove/Maths/MathsHelpers.hpp>

namespace clove {
    Text::Text(Font font)
        : font(std::move(font)) {
    }

    Text::Text(Text const &other) = default;

    Text::Text(Text &&other)  noexcept = default;

    Text &Text::operator=(Text const &other) = default;

    Text &Text::operator=(Text &&other)  noexcept = default;

    Text::~Text() = default;

    void Text::draw(vec2f const &drawSpace) {
        vec2f const screenHalfSize{ drawSpace.x / 2.0f, drawSpace.y / 2.0f };

        vec2f cursorPos{ position.x - screenHalfSize.x, -position.y + screenHalfSize.y };

        mat4f const projection{ createOrthographicMatrix(-screenHalfSize.x, screenHalfSize.x, -screenHalfSize.y, screenHalfSize.y) };

        for(size_t i = 0; i < getTextLength(); ++i) {
            Font::Glyph const &glyph{ getBufferForCharAt(i) };

            //For spaces we just skip and proceed
            if(glyph.character != nullptr) {
                uint32_t const width{ glyph.size.x };
                uint32_t const height{ glyph.size.y };

                float const xpos{ cursorPos.x + static_cast<float>(glyph.bearing.x) };
                float const ypos{ cursorPos.y - (static_cast<float>(height) - static_cast<float>(glyph.bearing.y)) };

                mat4f model{ translate(mat4f{ 1.0f }, { xpos, ypos, 0.0f }) };
                model *= scale(mat4f{ 1.0f }, { width, height, 0.0f });

                Application::get().getRenderer()->submitText(glyph.character, projection * model);
            }

            cursorPos.x += static_cast<float>(glyph.advance.x);
        }
    }

    Font::Glyph const &Text::getBufferForCharAt(size_t index) {
        if(isBufferDirty) {
            buildGlyphs();
        }
        return characters[index];
    }

    void Text::buildGlyphs() {
        characters.clear();
        for(char i : text) {
            characters.emplace_back(font.getChar(i));
        }
        isBufferDirty = false;
    }
}