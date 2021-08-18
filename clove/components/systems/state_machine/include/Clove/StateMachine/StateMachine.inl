#include "Clove/StateMachine/State.hpp"
#include "Clove/StateMachine/Transition.hpp"

namespace clove {
    template<typename Action>
    StateMachine<Action>::StateMachine(State<Action> *initialState)
        : currentState(initialState) {
    }

    template<typename Action>
    StateMachine<Action>::StateMachine(StateMachine<Action> &&other) noexcept;

    template<typename Action>
    StateMachine<Action> &StateMachine<Action>::operator=(StateMachine<Action> &&other) noexcept;

    template<typename Action>
    StateMachine<Action>::~StateMachine() = default;

    template<typename Action>
    std::vector<Action *> StateMachine<Action>::update() {
        Transition<Action> *triggeredTransition = nullptr;

        for(auto *transition : currentState->getTransitions()) {
            if(transition->isTriggered()) {
                triggeredTransition = transition;
                break;
            }
        }

        std::vector<Action *> actions;
        if(triggeredTransition != nullptr) {
            State<Action> *targetState = triggeredTransition->getState();

            auto stateExitActions        = currentState->getExitActions();
            auto transitionActions       = triggeredTransition->getActions();
            auto targetStateEnterActions = targetState->getEntryActions();

            actions.insert(actions.end(), stateExitActions.begin(), stateExitActions.end());
            actions.insert(actions.end(), transitionActions.begin(), transitionActions.end());
            actions.insert(actions.end(), targetStateEnterActions.begin(), targetStateEnterActions.end());

            currentState = targetState;
        } else {
            actions = currentState->getActions();
        }

        return actions;
    }
}