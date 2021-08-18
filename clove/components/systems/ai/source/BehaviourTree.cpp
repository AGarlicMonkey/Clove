#include "Clove/AI/BehaviourTree.hpp"

#include <utility>

namespace clove {
    BehaviourTree::BehaviourTree() = default;

    BehaviourTree::BehaviourTree(BehaviourTree &&other) noexcept = default;

    BehaviourTree &BehaviourTree::operator=(BehaviourTree &&other) noexcept = default;

    BehaviourTree::~BehaviourTree() = default;

    void BehaviourTree::setRootTask(std::unique_ptr<Task> task) {
        rootTask = std::move(task);
    }

    BlackBoard &BehaviourTree::getBlackBoard() {
        return blackboard;
    }

    void BehaviourTree::run(DeltaTime deltaTime) {
        if(rootTask != nullptr) {
            rootTask->activate(std::move(deltaTime), blackboard);
        }
    }
}