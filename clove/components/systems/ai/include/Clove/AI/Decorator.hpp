#pragma once

#include "Clove/AI/Task.hpp"

#include <memory>

namespace clove {
    class Decorator : public Task {
        //VARIABLES
    protected:
        std::unique_ptr<Task> child;

        //FUNCTIONS
    public:
        Decorator();

        Decorator(Decorator const &other) = delete;
        Decorator(Decorator &&other) noexcept;

        Decorator &operator=(Decorator const &other) = delete;
        Decorator &operator=(Decorator &&other) noexcept;

        virtual ~Decorator();

        void addChild(std::unique_ptr<Task> child);
    };
}