#include "Stem/Components/CameraComponent.hpp"

namespace garlic::inline stem {
    CameraComponent::CameraComponent(Camera camera)
        : camera(std::move(camera)) {
    }

    CameraComponent::CameraComponent(CameraComponent &&other) noexcept = default;

    CameraComponent &CameraComponent::operator=(CameraComponent &&other) noexcept = default;

    CameraComponent::~CameraComponent() = default;
}