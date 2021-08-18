#include <optional>

namespace clove {
    template<typename EventType>
    EventManager::EventContainer<EventType>::EventContainer() = default;

    template<typename EventType>
    EventManager::EventContainer<EventType>::EventContainer(EventContainer &&other) noexcept = default;

    template<typename EventType>
    EventManager::EventContainer<EventType> &EventManager::EventContainer<EventType>::operator=(EventContainer &&other) noexcept = default;

    template<typename EventType>
    EventManager::EventContainer<EventType>::~EventContainer() = default;

    template<typename EventType>
    EventHandle EventManager::EventContainer<EventType>::addListener(ContainerFunctionType function) {
        listeners.push_back(function);

        ListenerId const id{ nextId++ };
        listenerIdToIndexMap[id] = listeners.size() - 1;

        return { id, this };
    }

    template<typename EventType>
    void EventManager::EventContainer<EventType>::removeListener(ListenerId id) {
        if(auto iter = listenerIdToIndexMap.find(id); iter != listenerIdToIndexMap.end()) {
            size_t const index{ iter->second };
            size_t const lastIndex{ listeners.size() - 1 };

            std::optional<ListenerId> movedEvent;
            if(index < lastIndex) {
                for(auto &&idToIndexPair : listenerIdToIndexMap) {
                    if(idToIndexPair.second == lastIndex) {
                        movedEvent = idToIndexPair.first;
                        break;
                    }
                }

                listeners[iter->second] = listeners.back();
            }
            listeners.pop_back();
            listenerIdToIndexMap.erase(id);

            if(movedEvent) {
                listenerIdToIndexMap[*movedEvent] = index;
            }
        }
    }

    template<typename EventType>
    typename std::vector<typename EventManager::EventContainer<EventType>::ContainerFunctionType>::iterator EventManager::EventContainer<EventType>::begin() {
        return listeners.begin();
    }

    template<typename EventType>
    typename std::vector<typename EventManager::EventContainer<EventType>::ContainerFunctionType>::const_iterator EventManager::EventContainer<EventType>::begin() const {
        return listeners.begin();
    }

    template<typename EventType>
    typename std::vector<typename EventManager::EventContainer<EventType>::ContainerFunctionType>::iterator EventManager::EventContainer<EventType>::end() {
        return listeners.end();
    }

    template<typename EventType>
    typename std::vector<typename EventManager::EventContainer<EventType>::ContainerFunctionType>::const_iterator EventManager::EventContainer<EventType>::end() const {
        return listeners.end();
    }

    template<typename EventType>
    EventManager::EventContainer<EventType> &EventManager::getEventContainer() {
        const size_t eventId = typeid(EventType).hash_code();

        if(containers.find(eventId) == containers.end()) {
            containers[eventId] = std::make_unique<EventContainer<EventType>>();
        }

        return static_cast<EventContainer<EventType> &>(*containers[eventId]);
    }
}