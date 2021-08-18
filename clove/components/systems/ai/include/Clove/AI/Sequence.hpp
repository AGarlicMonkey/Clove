#pragma once

#include "Clove/AI/Composite.hpp"

#include <Clove/DeltaTime.hpp>

namespace clove {
    class Sequence : public Composite {
        //FUNCTIONS
    public:
        Sequence();

        Sequence(Sequence const &other) = delete;
        Sequence(Sequence &&other) noexcept;

        Sequence &operator=(Sequence const &other) = delete;
        Sequence &operator=(Sequence &&other) noexcept;

        ~Sequence();

        Status activate(DeltaTime const deltaTime, BlackBoard &blackboard) override;
    };
}