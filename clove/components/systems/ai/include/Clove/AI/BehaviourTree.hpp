#pragma once

#include "Clove/AI/Task.hpp"

#include <Clove/Blackboard.hpp>
#include <Clove/DeltaTime.hpp>
#include <memory>

namespace clove {
    class BehaviourTree {
        //VARIABLES
    private:
        Blackboard blackboard;
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

        Blackboard &getBlackboard();

        void run(DeltaTime deltaTime);
    };
}