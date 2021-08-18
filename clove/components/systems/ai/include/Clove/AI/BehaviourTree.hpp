#pragma once

#include "Clove/AI/BlackBoard.hpp"
#include "Clove/AI/Task.hpp"

#include <memory>
#include <Clove/DeltaTime.hpp>

namespace clove {
    class BehaviourTree {
        //VARIABLES
    private:
        BlackBoard blackboard;
        std::unique_ptr<Task> rootTask;

        //FUNCTIONS
    public:
        BehaviourTree();

        BehaviourTree(BehaviourTree const &other) = delete;
        BehaviourTree(BehaviourTree &&other) noexcept;

        BehaviourTree &operator=(BehaviourTree const &other) = delete;
        BehaviourTree &operator=(BehaviourTree &&other) noexcept;

        ~BehaviourTree();

        void setRootTask(std::unique_ptr<Task> task);

        BlackBoard &getBlackBoard();

        void run(DeltaTime deltaTime);
    };
}