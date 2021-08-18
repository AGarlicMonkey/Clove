#include "Clove/Event/EventManager.hpp"

namespace clove {
    EventManager::EventManager() = default;

    EventManager::EventManager(EventManager &&other) noexcept {
        containers = std::move(other.containers);
    }

    EventManager &EventManager::operator=(EventManager &&other) noexcept {
        containers = std::move(other.containers);

        return *this;
    }

    EventManager::~EventManager() = default;
}