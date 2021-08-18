#include "Clove/DeltaTime.hpp"

namespace clove {
    DeltaTime::DeltaTime(float deltaSeconds)
        : deltaSeconds(deltaSeconds) {
    }

    DeltaTime::DeltaTime() = default;

    DeltaTime::DeltaTime(DeltaTime const &other) = default;

    DeltaTime::DeltaTime(DeltaTime &&other) noexcept = default;

    DeltaTime &DeltaTime::operator=(DeltaTime const &other) = default;

    DeltaTime &DeltaTime::operator=(DeltaTime &&other) noexcept = default;
    
    float DeltaTime::getDeltaSeconds() const {
        return deltaSeconds;
    }

    DeltaTime::operator float() const {
        return deltaSeconds;
    }
}