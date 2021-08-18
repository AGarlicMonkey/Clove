#pragma once

namespace clove {
    template<typename... Types>
    struct Exclude {
        static size_t constexpr size{ sizeof...(Types) };
    };
}