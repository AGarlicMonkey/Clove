#pragma once

#include "Clove/Event/EventTypes.hpp"

namespace clove {
    class EventContainerBase;
}

namespace clove {
    struct EventHandle {
        friend class EventContainerBase;

        //VARIABLES
    private:
        ListenerId id{ invalidListenerId };
        EventContainerBase *container{ nullptr };

        //FUNCTIONS
    public:
        EventHandle();
        EventHandle(ListenerId id, EventContainerBase *container);

        EventHandle(EventHandle const &other) = delete;
        EventHandle(EventHandle &&other) noexcept;

        EventHandle &operator=(EventHandle const &other) = delete;
        EventHandle &operator=(EventHandle &&other) noexcept;

        ~EventHandle();

        void reset();

        ListenerId getId() const;

        bool isValid() const;

        operator ListenerId() const;

    private:
        void clear();
    };
}