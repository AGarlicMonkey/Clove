#include "Clove/Event/EventHandle.hpp"

#include "Clove/Event/EventContainerBase.hpp"

namespace clove {
    EventHandle::EventHandle() = default;

    EventHandle::EventHandle(ListenerId id, EventContainerBase *container)
        : id(id)
        , container(container) {
        this->container->handles.push_back(this);
    }

    EventHandle::EventHandle(EventHandle &&other) noexcept {
        reset();

        id        = other.id;
        container = other.container;
        container->handles.push_back(this);

        container->handles.erase(std::find(container->handles.begin(), container->handles.end(), &other));
        other.clear();
    }

    EventHandle &EventHandle::operator=(EventHandle &&other) noexcept {
        reset();

        id        = other.id;
        container = other.container;

        if(container != nullptr){
            container->handles.push_back(this);
            container->handles.erase(std::find(container->handles.begin(), container->handles.end(), &other));
        }

        other.clear();

        return *this;
    }

    EventHandle::~EventHandle() {
        reset();
    }

    void EventHandle::reset() {
        if(isValid()) {
            container->removeListener(id);
            container->handles.erase(std::find(container->handles.begin(), container->handles.end(), this));
            clear();
        }
    }

    ListenerId EventHandle::getId() const {
        return id;
    }

    bool EventHandle::isValid() const {
        return container != nullptr && id != invalidListenerId;
    }

    EventHandle::operator ListenerId() const {
        return getId();
    }

    void EventHandle::clear() {
        container = nullptr;
        id        = invalidListenerId;
    }
}