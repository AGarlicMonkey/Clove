namespace clove {
    template<typename Action>
    State<Action>::State(StateInitialiser<Action> &&initialiser) {
        entryActions = std::move(initialiser.entryActions);
        actions      = std::move(initialiser.actions);
        exitActions  = std::move(initialiser.exitActions);

        transitions = std::move(initialiser.transitions);
    }

    template<typename Action>
    State<Action>::State(State &&other) noexcept = default;

    template<typename Action>
    State<Action> &State<Action>::operator=(State<Action> &&other) noexcept;

    template<typename Action>
    State<Action>::~State() = default;

    template<typename Action>
    std::vector<Action *> State<Action>::getEntryActions() {
        std::vector<Action *> out;
        std::transform(std::begin(entryActions), std::end(entryActions), std::back_inserter(out), [](auto &action) { return action.get(); });
        return out;
    }

    template<typename Action>
    std::vector<Action *> State<Action>::getActions() {
        std::vector<Action *> out;
        std::transform(std::begin(actions), std::end(actions), std::back_inserter(out), [](auto &action) { return action.get(); });
        return out;
    }

    template<typename Action>
    std::vector<Action *> State<Action>::getExitActions() {
        std::vector<Action *> out;
        std::transform(std::begin(exitActions), std::end(exitActions), std::back_inserter(out), [](auto &action) { return action.get(); });
        return out;
    }

    template<typename Action>
    std::vector<Transition<Action> *> State<Action>::getTransitions() {
        std::vector<Transition<Action> *> out;
        std::transform(std::begin(transitions), std::end(transitions), std::back_inserter(out), [](auto &action) { return action.get(); });
        return out;
    }
}