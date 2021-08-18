namespace clove {
    template<typename EventType>
    EventHandle EventDispatcher::bindToEvent(ListenerFunction<EventType> &&function) {
        return manager.getEventContainer<EventType>().addListener(std::forward<ListenerFunction<EventType>>(function));
    }

    template<typename EventType>
    void EventDispatcher::unbindFromEvent(ListenerId id) {
        manager.getEventContainer<EventType>().removeListener(id);
    }

    template<typename EventType>
    void EventDispatcher::broadCastEvent(EventType const &event) {
        for(auto &&func : manager.getEventContainer<EventType>()) {
            func(event);
        }
    }
}