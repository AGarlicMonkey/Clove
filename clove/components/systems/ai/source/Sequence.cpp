#include "Clove/AI/Sequence.hpp"

namespace clove {
    Sequence::Sequence() = default;

    Sequence::Sequence(Sequence &&other) noexcept = default;

    Sequence &Sequence::operator=(Sequence &&other) noexcept = default;

    Sequence::~Sequence() = default;

    Task::Status Sequence::activate(DeltaTime const deltaTime, BlackBoard &blackboard) {
        for(auto const &child : children) {
            Status const status{ child->activate(deltaTime, blackboard) };
            if(status != Status::Success) {
                return status;
            }
        }
        return Status::Success;
    }
}