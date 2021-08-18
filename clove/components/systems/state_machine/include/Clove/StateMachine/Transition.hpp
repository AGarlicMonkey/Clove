#include <functional>
#include <memory>
#include <vector>

namespace clove {
    template<typename Action>
    class State;
}

namespace clove {
    /**
	 * @brief A Transition defines a condition on how to transition from one State to another
	 * @details Transitions can also perform actions while transitioning between States
	 * @tparam Action The type of Action this Transition deals with
	 */
    template<typename Action>
    class Transition {
        //VARIABLES
    private:
        //TODO: custom deleter for unique ptrs so allocators can be used
        std::function<bool()> condition;
        State<Action> *state;
        std::vector<std::unique_ptr<Action>> actions;

        //FUNCTIONS
    public:
        Transition();

        Transition(Transition const &other) = delete;
        Transition(Transition &&other) noexcept;

        Transition &operator=(Transition const &other) = delete;
        Transition &operator=(Transition &&other) noexcept;

        ~Transition();

        void init(std::function<bool()> condition, State<Action> *state);
        void init(std::function<bool()> condition, State<Action> *state, std::vector<std::unique_ptr<Action>> actions);

        bool isTriggered() const;

        State<Action> *getState() const;
        std::vector<Action *> getActions() const;
    };
}

#include "Transition.inl"
