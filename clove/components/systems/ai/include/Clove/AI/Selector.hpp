#pragma once

#include "Clove/AI/Composite.hpp"

#include <Clove/DeltaTime.hpp>

namespace clove {
    class Selector : public Composite {
        //FUNCTIONS
    public:
        Selector();

        Selector(Selector const &other) = delete;
        Selector(Selector &&other) noexcept;

        Selector &operator=( Selector const &other) = delete;
        Selector &operator=(Selector &&other) noexcept;

        ~Selector();

        Status activate(DeltaTime const deltaTime, BlackBoard &blackboard) override;
    };
}