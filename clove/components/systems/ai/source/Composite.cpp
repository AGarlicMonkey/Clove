#include "Clove/AI/Composite.hpp"

namespace clove {
    Composite::Composite() = default;

    Composite::Composite(Composite &&other) noexcept = default;

    Composite &Composite::operator=(Composite &&other) noexcept = default;

    Composite::~Composite() = default;

    void Composite::addChild(std::unique_ptr<Task> child) {
        children.push_back(std::move(child));
    }
}