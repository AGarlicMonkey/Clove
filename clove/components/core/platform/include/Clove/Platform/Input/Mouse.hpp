#pragma once

//There seems to be a bug with optional in msvc that stops it compiling. Having this header included here (before anything) is a current work around.
#include <optional>
//

#include "Clove/Platform/Input/MouseButtonCodes.hpp"

#include <Clove/Maths/Vector.hpp>
#include <bitset>
#include <queue>
#include <unordered_map>

namespace clove {
    /**
     * @brief Provides an interface to handle any platform mouse events.
     */
    class Mouse {
        //TYPES
    public:
        /**
         * @brief Represents a single mouse event (clicked, moved etc.)
         */
        struct Event {
            //TYPES
        public:
            enum class Type {
                Move,
                Pressed,
                Released,
                WheelUp,
                WheelDown,
                Enter,
                Leave,
                Invalid,
            };

            //VARIABLES
        private:
            Type type{ Type::Invalid };
            MouseButton button{ MouseButton::None };

            vec2i pos{};

            //FUNCTIONS
        public:
            Event();
            Event(Type type, MouseButton button, vec2i pos);

            Type getType() const;
            bool isValid() const;

            /**
             * @brief Returns the position in the window where this event happened.
             */
            vec2i const &getPos() const;

            MouseButton getButton() const;
        };

        /**
         * @brief Recieves events for a Mouse object 
         */
        class Dispatcher {
            friend Mouse;

            //VARIABLES
        private:
            static constexpr uint32_t bufferSize{ 16u };

            int32_t wheelDelta{ 0 };
            vec2i lastPos{ 0 };

            bool inWindow{ false };

            std::unordered_map<MouseButton, bool> buttonStates;
            std::queue<Event> buffer;

            //FUNCTIONS
        public:
            Dispatcher();

            Dispatcher(Dispatcher const &other);
            Dispatcher(Dispatcher &&other) noexcept;

            Dispatcher &operator=(Dispatcher const &other);
            Dispatcher &operator=(Dispatcher &&other) noexcept;

            ~Dispatcher();

            void onMouseMove(vec2i pos);

            void onButtonPressed(MouseButton button, vec2i pos);
            void onButtonReleased(MouseButton button, vec2i pos);

            void onWheelDelta(int32_t delta, vec2i pos);
            void onWheelUp(vec2i pos);
            void onWheelDown(vec2i pos);

            void onMouseLeave();
            void onMouseEnter();

            void clearState();

            void trimBuffer();
        };

        //VARIABLES
    private:
        Dispatcher &dispatcher;

        //FUNCTIONS
    public:
        Mouse() = delete;
        Mouse(Dispatcher &dispatcher);

        Mouse(Mouse const &other)     = delete;
        Mouse(Mouse &&other) noexcept = delete;

        Mouse &operator=(Mouse const &other) = delete;
        Mouse &operator=(Mouse &&other) noexcept = delete;

        ~Mouse() = default;

        /**
         * @brief Pops an event off of the buffer and returns it.
         * @return An event from the front of the buffer. std::nullopt if buffer is empty.
         */
        std::optional<Event> getEvent();

        bool isButtonPressed(MouseButton button) const;
        
        /**
         * @brief Gets the current position of the mouse in screen coordinates.
         * @details This is the current position at the time of calling this function
         * and not the position of the last event. This can be useful when setting the position
         * manually and mouse move events need to be ignored.
         */
        vec2i getPosition() const;
        /**
         * @brief Sets the position of the mouse in screen coordinates. This will generate a mouse move event.
         */
        void setPosition(vec2i const &position);

        /**
         * @brief Shows / hides the mouse.
         */
        void show(bool shouldShow);

        bool isInWindow() const;

        bool isBufferEmpty() const;

        void flush();
    };
}
