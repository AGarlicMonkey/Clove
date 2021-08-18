#include "Clove/Event/EventContainerBase.hpp"

#include "Clove/Event/EventHandle.hpp"

namespace clove {
    EventContainerBase::EventContainerBase() = default;

    EventContainerBase::EventContainerBase(EventContainerBase &&other) noexcept {
        handles = std::move(other.handles);

        for(auto *handle : handles) {
            handle->container = this;
        }
    }

    EventContainerBase &EventContainerBase::operator=(EventContainerBase &&other) noexcept {
        handles = std::move(other.handles);

        for(auto *handle : handles) {
            handle->container = this;
        }

        return *this;
    }

    EventContainerBase::~EventContainerBase() {
        for(auto *handle : handles) {
            handle->clear();
        }
    }
}