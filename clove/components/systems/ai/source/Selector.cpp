#include "Clove/AI/Selector.hpp"

namespace clove {
    Selector::Selector() = default;

    Selector::Selector(Selector &&other) noexcept = default;

    Selector &Selector::operator=(Selector &&other) noexcept = default;

    Selector::~Selector() = default;

    Task::Status Selector::activate(DeltaTime const deltaTime, BlackBoard &blackboard) {
        for(auto const &child : children) {
            Status const status{ child->activate(deltaTime, blackboard) };
            if(status != Status::Failure) {
                return status;
            }
        }
        return Status::Failure;
    }
}