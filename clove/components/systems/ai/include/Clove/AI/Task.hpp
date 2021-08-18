#pragma once

#include <Clove/DeltaTime.hpp>

namespace clove {
    class BlackBoard;
}

namespace clove {
    /**
	 * @brief a Task is an action an AI agent can perform. Determined by a behaviour tree
	 */
    class Task {
        //TYPES
    public:
        /**
		 * @brief Represents the status of a task after activating it
		 */
        enum class Status {
            Running, /**< Signifies this task is still running. The tree will re-traverse itself up to this task next update */
            Success, /**< Signifies that this task finished successfully */
            Failure  /**< Signifies that this task did not finished successfully */
        };

        //FUNCTIONS
    public:
        virtual ~Task() = default;

        virtual Status activate(DeltaTime const deltaTime, BlackBoard &blackboard) = 0;
    };
}