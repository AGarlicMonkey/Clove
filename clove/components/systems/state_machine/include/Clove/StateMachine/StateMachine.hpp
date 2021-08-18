#pragma once

namespace clove {
    template<typename Action>
    class State;
}

namespace clove {
    /**
	 * @brief A StateMachine manages States and Transitions. Determining what Action to execute.
	 * @tparam Action The type of Action this StateMachine handles
	 */
    template<typename Action>
    class StateMachine {
        //VARIABLES
    private:
        State<Action> *currentState = nullptr;

        //FUNCTIONS
    public:
        StateMachine() = delete;
        StateMachine(State<Action> *initialState);

        StateMachine(StateMachine const &other) = delete;
        StateMachine(StateMachine &&other) noexcept;

        StateMachine &operator=(StateMachine const &other) = delete;
        StateMachine &operator=(StateMachine &&other) noexcept;

        ~StateMachine();

        /**
		 * @brief Runs through the current State to test for any Transitions that were activated
		 * @return A vector of Actions to perform
		 */
        std::vector<Action *> update();
    };
}

#include "StateMachine.inl"
