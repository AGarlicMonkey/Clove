#include "Clove/AI/Decorator.hpp"

namespace clove {
    Decorator::Decorator() = default;

    Decorator::Decorator(Decorator &&other) noexcept = default;

    Decorator &Decorator::operator=(Decorator &&other) noexcept = default;

    Decorator::~Decorator() = default;

    void Decorator::addChild(std::unique_ptr<Task> child) {
        this->child = std::move(child);
    }
}