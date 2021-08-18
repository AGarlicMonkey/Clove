#pragma once

#include "Clove/Platform/Input/KeyCodes.hpp"

#include <bitset>
#include <optional>
#include <queue>
#include <unordered_map>

namespace clove {
    /**
     * @brief Provides an interface to handle any platform keyboard events.
     */
    class Keyboard {
        //TYPES
    public:
        /**
         * @brief Represents a single key event (pressed, released etc.)
         */
        struct Event {
            //TYPES
        public:
            enum class Type {
                Pressed,
                Released,
                Invalid
            };

            //VARIABLES
        private:
            Type type{ Type::Invalid };
            Key key{ Key::None };

            //FUNCTIONS
        public:
            Event();
            Event(Type type, Key key);

            bool isPressed() const;
            bool isReleased() const;
            bool isValid() const;

            Type getType() const;
            Key getKey() const;
        };

        /**
         * @brief Recieves events for a Keyboard object 
         */
        class Dispatcher {
            friend Keyboard;

            //VARIABLES
        private:
            static constexpr uint32_t bufferSize{ 16u };

            bool autoRepeatEnabled{ false };

            std::unordered_map<Key, bool> keyStates;
            std::queue<Event> keyBuffer;
            std::queue<char> charBuffer;

            //FUNCTIONS
        public:
            Dispatcher();

            Dispatcher(Dispatcher const &other);
            Dispatcher(Dispatcher &&other) noexcept;

            Dispatcher &operator=(Dispatcher const &other);
            Dispatcher &operator=(Dispatcher &&other) noexcept;

            ~Dispatcher();

            void onKeyPressed(Key key);
            void onKeyReleased(Key key);

            void onChar(char character);

            void clearState();

            template<typename T>
            static void trimBuffer(std::queue<T> &buffer);
        };

        //VARIABLES
    private:
        Dispatcher &dispatcher;

        //FUNCTIONS
    public:
        Keyboard() = delete;
        Keyboard(Dispatcher &dispatcher);

        Keyboard(Keyboard const &other)     = delete;
        Keyboard(Keyboard &&other) noexcept = delete;

        Keyboard &operator=(Keyboard const &other) = delete;
        Keyboard &operator=(Keyboard &&other) noexcept = delete;

        ~Keyboard() = default;

        bool isKeyPressed(Key key) const;
        std::optional<Event> getKeyEvent();
        bool isKeyBufferEmpty() const;

        std::optional<char> getCharEvent();
        bool isCharBufferEmpty() const;

        void flushKeyBuffer();
        void flushCharBuffer();
        void flush();

        void enableAutoRepeat();
        void disableAutoRepeat();
        bool isAutoRepeatEnabled() const;
    };
}

#include "Keyboard.inl"
