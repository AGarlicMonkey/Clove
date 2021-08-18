#include "Clove/Platform/Input/Keyboard.hpp"

namespace clove {
    Keyboard::Event::Event() = default;

    Keyboard::Event::Event(Type type, Key key)
        : type(type)
        , key(key) {
    }

    bool Keyboard::Event::isPressed() const {
        return type == Type::Pressed;
    }

    bool Keyboard::Event::isReleased() const {
        return type == Type::Released;
    }

    bool Keyboard::Event::isValid() const {
        return type != Type::Invalid && key > Key::Undefined;
    }

    Keyboard::Event::Type Keyboard::Event::getType() const {
        return type;
    }

    Key Keyboard::Event::getKey() const {
        return key;
    }

    Keyboard::Dispatcher::Dispatcher() = default;

    Keyboard::Dispatcher::Dispatcher(Dispatcher const &other) = default;

    Keyboard::Dispatcher::Dispatcher(Dispatcher &&other) noexcept = default;

    Keyboard::Dispatcher &Keyboard::Dispatcher::operator=(Dispatcher const &other) = default;

    Keyboard::Dispatcher &Keyboard::Dispatcher::operator=(Dispatcher &&other) noexcept = default;

    Keyboard::Dispatcher::~Dispatcher() = default;

    void Keyboard::Dispatcher::onKeyPressed(Key key) {
        keyStates[key] = true;
        keyBuffer.push({ Keyboard::Event::Type::Pressed, key });
        trimBuffer(keyBuffer);
    }

    void Keyboard::Dispatcher::onKeyReleased(Key key) {
        keyStates[key] = false;
        keyBuffer.push({ Keyboard::Event::Type::Released, key });
        trimBuffer(keyBuffer);
    }

    void Keyboard::Dispatcher::onChar(char character) {
        charBuffer.push(character);
        trimBuffer(charBuffer);
    }

    void Keyboard::Dispatcher::clearState() {
        keyStates.clear();
    }

    Keyboard::Keyboard(Dispatcher &dispatcher)
        : dispatcher{ dispatcher } {
    }

    bool Keyboard::isKeyPressed(Key key) const {
        if(const auto keyIt = dispatcher.keyStates.find(key); keyIt != dispatcher.keyStates.end()) {
            return keyIt->second;
        } else {
            return false;
        }
    }

    bool Keyboard::isKeyBufferEmpty() const {
        return dispatcher.keyBuffer.empty();
    }

    std::optional<Keyboard::Event> Keyboard::getKeyEvent() {
        if(!isKeyBufferEmpty()) {
            Keyboard::Event e{ dispatcher.keyBuffer.front() };
            dispatcher.keyBuffer.pop();
            return e;
        } else {
            return {};
        }
    }

    std::optional<char> Keyboard::getCharEvent() {
        if(!isCharBufferEmpty()) {
            char charCode{ dispatcher.charBuffer.front() };
            dispatcher.charBuffer.pop();
            return charCode;
        } else {
            return {};
        }
    }

    void Keyboard::flushKeyBuffer() {
        dispatcher.keyBuffer = std::queue<Event>();
    }

    void Keyboard::flushCharBuffer() {
        dispatcher.charBuffer = std::queue<char>();
    }

    bool Keyboard::isCharBufferEmpty() const {
        return dispatcher.charBuffer.empty();
    }

    void Keyboard::flush() {
        flushKeyBuffer();
        flushCharBuffer();
    }

    void Keyboard::enableAutoRepeat() {
        dispatcher.autoRepeatEnabled = true;
    }

    void Keyboard::disableAutoRepeat() {
        dispatcher.autoRepeatEnabled = false;
    }

    bool Keyboard::isAutoRepeatEnabled() const {
        return dispatcher.autoRepeatEnabled;
    }
}
