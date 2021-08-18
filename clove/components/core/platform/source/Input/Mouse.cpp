#include "Clove/Platform/Input/Mouse.hpp"

namespace clove {
    Mouse::Event::Event() = default;

    Mouse::Event::Event(Type type, MouseButton button, vec2i pos)
        : type{ type }
        , button{ button }
        , pos{ pos } {
    }

    Mouse::Event::Type Mouse::Event::getType() const {
        return type;
    }

    bool Mouse::Event::isValid() const {
        return type != Type::Invalid && button > MouseButton::Undefined;
    }

    vec2i const &Mouse::Event::getPos() const {
        return pos;
    }

    MouseButton Mouse::Event::getButton() const {
        return button;
    }

    Mouse::Dispatcher::Dispatcher() = default;

    Mouse::Dispatcher::Dispatcher(Dispatcher const &other) = default;

    Mouse::Dispatcher::Dispatcher(Dispatcher &&other) noexcept = default;

    Mouse::Dispatcher &Mouse::Dispatcher::operator=(Dispatcher const &other) = default;

    Mouse::Dispatcher &Mouse::Dispatcher::operator=(Dispatcher &&other) noexcept = default;

    Mouse::Dispatcher::~Dispatcher() = default;

    void Mouse::Dispatcher::onMouseMove(vec2i pos) {
        lastPos = pos;
        
        buffer.push({ Mouse::Event::Type::Move, MouseButton::None, pos });
        trimBuffer();
    }

    void Mouse::Dispatcher::onButtonPressed(MouseButton button, vec2i pos) {
        lastPos = pos;

        buttonStates[button] = true;
        buffer.push({ Mouse::Event::Type::Pressed, button, pos });
        trimBuffer();
    }

    void Mouse::Dispatcher::onButtonReleased(MouseButton button, vec2i pos) {
        lastPos = pos;

        buttonStates[button] = false;
        buffer.push({ Mouse::Event::Type::Released, button, pos });
        trimBuffer();
    }

    void Mouse::Dispatcher::onWheelDelta(int32_t delta, vec2i pos) {
        lastPos = pos;

        wheelDelta += delta;
        while(wheelDelta >= CLV_WHEEL_DELTA) {
            wheelDelta -= CLV_WHEEL_DELTA;
            onWheelUp(pos);
        }
        while(wheelDelta <= -CLV_WHEEL_DELTA) {
            wheelDelta += CLV_WHEEL_DELTA;
            onWheelDown(pos);
        }
    }

    void Mouse::Dispatcher::onWheelUp(vec2i pos) {
        lastPos = pos;

        buffer.push({ Mouse::Event::Type::WheelUp, MouseButton::None, pos });
        trimBuffer();
    }

    void Mouse::Dispatcher::onWheelDown(vec2i pos) {
        lastPos = pos;

        buffer.push({ Mouse::Event::Type::WheelDown, MouseButton::None, pos });
        trimBuffer();
    }

    void Mouse::Dispatcher::onMouseLeave() {
        inWindow = false;

        buffer.push({ Mouse::Event::Type::Leave, MouseButton::None, lastPos });
        trimBuffer();
    }

    void Mouse::Dispatcher::onMouseEnter() {
        inWindow = true;

        buffer.push({ Mouse::Event::Type::Enter, MouseButton::None, lastPos });
        trimBuffer();
    }

    void Mouse::Dispatcher::clearState() {
        buttonStates.clear();
    }

    void Mouse::Dispatcher::trimBuffer() {
        while(buffer.size() > bufferSize) {
            buffer.pop();
        }
    }

    Mouse::Mouse(Dispatcher &dispatcher)
        : dispatcher{ dispatcher } {
    }

    bool Mouse::isButtonPressed(MouseButton button) const {
        if(const auto buttonIt = dispatcher.buttonStates.find(button); buttonIt != dispatcher.buttonStates.end()) {
            return buttonIt->second;
        } else {
            return false;
        }
    }

    std::optional<Mouse::Event> Mouse::getEvent() {
        if(!isBufferEmpty()) {
            Mouse::Event e{ dispatcher.buffer.front() };
            dispatcher.buffer.pop();
            return e;
        } else {
            return {};
        }
    }

    bool Mouse::isInWindow() const {
        return dispatcher.inWindow;
    }

    bool Mouse::isBufferEmpty() const {
        return dispatcher.buffer.empty();
    }

    void Mouse::flush() {
        dispatcher.buffer = std::queue<Mouse::Event>();
    }
}
