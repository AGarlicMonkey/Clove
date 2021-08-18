#pragma once

#include <functional>

namespace clove {
    using ListenerId = size_t;
    static constexpr ListenerId invalidListenerId{ 0 };

    template<typename EventType>
    using ListenerFunction = std::function<void(EventType const &)>;
}