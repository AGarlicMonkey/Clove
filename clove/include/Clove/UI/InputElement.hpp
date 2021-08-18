#pragma once

#include "Clove/InputResponse.hpp"

#include <Clove/Platform/Input/Keyboard.hpp>
#include <Clove/Platform/Input/Mouse.hpp>

namespace clove {
    struct ElementBounds {
        vec2i start;
        vec2i end;
    };
}

namespace clove {
    class InputElement {
        //FUNCTIONS
    public:
        virtual ~InputElement() = default;

        virtual InputResponse onKeyEvent(Keyboard::Event const &event) = 0;
        virtual InputResponse onMouseEvent(Mouse::Event const &event)  = 0;

        virtual ElementBounds getBounds() const = 0;
    };
}