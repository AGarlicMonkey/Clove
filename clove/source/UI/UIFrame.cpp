#include "Clove/UI/UIFrame.hpp"

#include "Clove/UI/DrawableElement.hpp"
#include "Clove/UI/InputElement.hpp"

namespace clove {
    UIFrame::UIFrame(vec2i size)
        : size(size) {
    }

    InputResponse UIFrame::propagateInput(InputEvent const &inputEvent) {
        if(auto const *const mouseEvent{ std::get_if<Mouse::Event>(&inputEvent) }){
            for(auto &element : inputElements) {
                ElementBounds const bounds{element->getBounds()};
                auto const pos{mouseEvent->getPos()};

                if(pos.x >= bounds.start.x && pos.x <= bounds.end.x &&
                   pos.y >= bounds.start.y && pos.y <= bounds.end.y) {
                    if(element->onMouseEvent(*mouseEvent) == InputResponse::Consumed) {
                        return InputResponse::Consumed;
                    }
                }
            }
        }
        //TODO: Keyboard event (requires focus)

        return InputResponse::Ignored;
    }

    void UIFrame::drawChildren() {
        for(auto &element : drawables) {
            element->draw(size);
        }
    }
}
