#pragma once

#include <Clove/Platform/Input/Keyboard.hpp>
#include <Clove/Platform/Input/Mouse.hpp>
#include <variant>

namespace clove {
    struct InputEvent {
        enum class Type {
            Keyboard,
            Mouse
        };

        std::variant<Keyboard::Event, Mouse::Event> event;
        Type eventType;
    };
}