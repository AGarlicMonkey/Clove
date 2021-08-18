#include "Clove/UI/UIFrame.hpp"

#include "Clove/UI/DrawableElement.hpp"
#include "Clove/UI/InputElement.hpp"

namespace clove {
    UIFrame::UIFrame(vec2i size)
        : size(size) {
    }

    InputResponse UIFrame::propagateInput(InputEvent const &inputEvent) {
        switch(inputEvent.eventType) {
            case InputEvent::Type::Keyboard:
                //TODO: Needs to check if the elemet has focus
                break;

            case InputEvent::Type::Mouse: {
                auto const &mouseEvent = std::get<Mouse::Event>(inputEvent.event);
                for(auto &element : inputElements) {
                    ElementBounds const bounds{element->getBounds()};
                    auto const pos{mouseEvent.getPos()};

                    if(pos.x >= bounds.start.x && pos.x <= bounds.end.x &&
                       pos.y >= bounds.start.y && pos.y <= bounds.end.y) {
                        if(element->onMouseEvent(mouseEvent) == InputResponse::Consumed) {
                            return InputResponse::Consumed;
                        }
                    }
                }
            } break;
            default:
                break;
        }

        return InputResponse::Ignored;
    }

    void UIFrame::drawChildren() {
        for(auto &element : drawables) {
            element->draw(size);
        }
    }
}