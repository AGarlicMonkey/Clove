#pragma once

#include "Clove/InputResponse.hpp"
#include "Clove/InputEvent.hpp"

#include <memory>

namespace clove {
    class DrawableElement;
    class InputElement;
}

namespace clove {
    class UIFrame {
        //VARIABLES
    private:
        //SharedPtr for now, good candidate for an allocator to get cache friendly
        std::vector<std::shared_ptr<DrawableElement>> drawables;
        std::vector<std::shared_ptr<InputElement>> inputElements;

        vec2i size;

        //FUNCTIONS
    public:
        UIFrame() = delete;
        UIFrame(vec2i size);

        InputResponse propagateInput(InputEvent const &inputEvent);

        void drawChildren();

        template<typename Element>
        void addElement(std::shared_ptr<Element> element) {
            if constexpr(std::is_base_of_v<DrawableElement, Element>) {
                drawables.push_back(element);
            }
            if constexpr(std::is_base_of_v<InputElement, Element>) {
                inputElements.push_back(element);
            }
        }
    };
}