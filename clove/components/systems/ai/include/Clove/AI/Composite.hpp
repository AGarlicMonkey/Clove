#pragma once

#include "Clove/AI/Task.hpp"

#include <vector>
#include <memory>

namespace clove {
    class Composite : public Task {
        //VARIABLES
    protected:
        std::vector<std::unique_ptr<Task>> children;

        //FUNCTIONS
    public:
        Composite();

        Composite(Composite const &other) = delete;
        Composite(Composite &&other) noexcept;

        Composite &operator=(Composite const &other) = delete;
        Composite &operator=(Composite &&other) noexcept;

        virtual ~Composite();

        void addChild(std::unique_ptr<Task> child);
    };
}