#pragma once

#include <concepts>

namespace clove {
    template<typename T>
    concept number = std::integral<T> || std::floating_point<T>;
}