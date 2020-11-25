#include "Clove/Components/StaticModelComponent.hpp"

#include "Clove/Rendering/Renderables/Mesh.hpp"

namespace garlic::clove {
    StaticModelComponent::StaticModelComponent(StaticModel model)
        : model(std::move(model)) {
    }

    StaticModelComponent::StaticModelComponent(StaticModelComponent const &other) = default;

    StaticModelComponent::StaticModelComponent(StaticModelComponent &&other) noexcept
        : model(std::move(other.model)) {
    }

    StaticModelComponent &StaticModelComponent::operator=(StaticModelComponent const &other) = default;

    StaticModelComponent &StaticModelComponent::operator=(StaticModelComponent &&other) noexcept {
        model = other.model;
        return *this;
    }

    StaticModelComponent::~StaticModelComponent() = default;
}