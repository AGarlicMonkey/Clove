#pragma once

#include "Clove/Event/EventManager.hpp"
#include "Clove/Event/EventTypes.hpp"

namespace clove {
    class EventDispatcher {
        //VARIABLES
    private:
        EventManager manager;

        //FUNCTIONS
    public:
        EventDispatcher();

        EventDispatcher(EventDispatcher const &other) = delete;
        EventDispatcher(EventDispatcher &&other) noexcept;

        EventDispatcher &operator=(EventDispatcher const &other) = delete;
        EventDispatcher &operator=(EventDispatcher &&other) noexcept;

        ~EventDispatcher();

        template<typename EventType>
        [[nodiscard]] EventHandle bindToEvent(ListenerFunction<EventType> &&function);
        template<typename EventType>
        void unbindFromEvent(ListenerId id);

        template<typename EventType>
        void broadCastEvent(EventType const &event);
    };
}

#include "EventDispatcher.inl"