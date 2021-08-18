#include "Clove/Platform/Input/Mouse.hpp"

namespace clove {
    vec2i Mouse::getPosition() const {
        POINT point{};
        GetCursorPos(&point);

        return { point.x, point.y };
    }

    void Mouse::setPosition(vec2i const &position) {
        SetCursorPos(position.x, position.y);
    }

    void Mouse::show(bool shouldShow) {
        ShowCursor(shouldShow);
    }
}