#include <memory>
#include <vector>

namespace clove {
    template<typename Action>
    class Transition;

    /**
	 * @brief A helper struct to avoid cyclic dependencies when initialising States
	 */
    template<typename Action>
    struct StateInitialiser {
        std::vector<std::unique_ptr<Action>> entryActions;
        std::vector<std::unique_ptr<Action>> actions;
        std::vector<std::unique_ptr<Action>> exitActions;

        std::vector<std::unique_ptr<Transition<Action>>> transitions;
    };
}

namespace clove {
    /**
	 * @brief A State is a State that a StateMachine can currently be in.
	 * @details A State is comprised of entry Actions, Actions and exit Actions. These are certain behaviours
	 * that are to be executed when entering this State, updating this State and exiting this State.
	 * How this is done is governed by a list of Transitions.
	 * @tparam Action The type of Action this State deals with
	 * @see	Transition
	 */
    template<typename Action>
    class State {
        //VARIABLES
    private:
        //TODO: custom deleter for unique ptrs so allocators can be used
        std::vector<std::unique_ptr<Action>> entryActions;
        std::vector<std::unique_ptr<Action>> actions;
        std::vector<std::unique_ptr<Action>> exitActions;

        std::vector<std::unique_ptr<Transition<Action>>> transitions;

        //FUNCTIONS
    public:
        State() = delete;
        State(StateInitialiser<Action> &&initialiser);

        State(State const &other) = delete;
        State(State &&other) noexcept;

        State &operator=(State const &other) = delete;
        State &operator=(State &&other) noexcept;

        ~State();

        std::vector<Action *> getEntryActions();
        std::vector<Action *> getActions();
        std::vector<Action *> getExitActions();

        std::vector<Transition<Action> *> getTransitions();
    };
}

#include "State.inl"
