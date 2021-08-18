namespace clove {
    template<typename Action>
    Transition<Action>::Transition() = default;

    template<typename Action>
    Transition<Action>::Transition(Transition<Action> &&other) noexcept = default;

    template<typename Action>
    Transition<Action> &Transition<Action>::operator=(Transition<Action> &&other) noexcept = default;

    template<typename Action>
    Transition<Action>::~Transition() = default;

    template<typename Action>
    void Transition<Action>::init(std::function<bool()> condition, State<Action> *state) {
        this->condition = std::move(condition);
        this->state     = state;
    }

    template<typename Action>
    void Transition<Action>::init(std::function<bool()> condition, State<Action> *state, std::vector<std::unique_ptr<Action>> actions) {
        this->condition = std::move(condition);
        this->state     = state;
        this->actions   = std::move(actions);
    }

    template<typename Action>
    bool Transition<Action>::isTriggered() const {
        return condition();
    }

    template<typename Action>
    State<Action> *Transition<Action>::getState() const {
        return state;
    }

    template<typename Action>
    std::vector<Action *> Transition<Action>::getActions() const {
        std::vector<Action *> out;
        std::transform(std::begin(actions), std::end(actions), std::back_inserter(out), [](auto &action) { return action.get(); });
        return out;
    }
}