#pragma once

#include "Clove/Event/EventContainerBase.hpp"
#include "Clove/Event/EventHandle.hpp"
#include "Clove/Event/EventTypes.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace clove {
    class EventManager {
        //TYPES
    private:
        template<typename EventType>
        class EventContainer : public EventContainerBase {
            //TYPES
        private:
            using ContainerFunctionType = ListenerFunction<EventType>;

            //VARIABLES
        private:
            std::unordered_map<ListenerId, size_t> listenerIdToIndexMap;
            std::vector<ContainerFunctionType> listeners;

            //FUNCTIONS
        public:
            EventContainer();

            EventContainer(EventContainer const &other) = delete;
            EventContainer(EventContainer &&other) noexcept;

            EventContainer &operator=(EventContainer const &other) = delete;
            EventContainer &operator=(EventContainer &&other) noexcept;

            ~EventContainer();

            [[nodiscard]] EventHandle addListener(ContainerFunctionType function);
            void removeListener(ListenerId id) final;

            typename std::vector<ContainerFunctionType>::iterator begin();
            typename std::vector<ContainerFunctionType>::const_iterator begin() const;

            typename std::vector<ContainerFunctionType>::iterator end();
            typename std::vector<ContainerFunctionType>::const_iterator end() const;
        };

        //VARIABLES
    private:
        std::unordered_map<size_t, std::unique_ptr<EventContainerBase>> containers;

        //FUNCTIONS
    public:
        EventManager();

        EventManager(EventManager const &other) = delete;
        EventManager(EventManager &&other) noexcept;

        EventManager &operator=(EventManager const &other) = delete;
        EventManager &operator=(EventManager &&other) noexcept;

        ~EventManager();

        template<typename EventType>
        EventContainer<EventType> &getEventContainer();
    };
}

#include "EventManager.inl"