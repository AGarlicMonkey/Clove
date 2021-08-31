#pragma once

#include <Clove/Platform/Input/Keyboard.hpp>
#include <Clove/Platform/Input/Mouse.hpp>
#include <variant>

namespace clove {
    using InputEvent = std::variant<Keyboard::Event, Mouse::Event>;
}
