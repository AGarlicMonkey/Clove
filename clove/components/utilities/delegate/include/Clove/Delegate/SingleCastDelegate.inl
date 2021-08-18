namespace clove {
    template<typename FunctionPrototype>
    SingleCastDelegate<FunctionPrototype>::SingleCastDelegate() = default;

    template<typename FunctionPrototype>
    SingleCastDelegate<FunctionPrototype>::SingleCastDelegate(SingleCastDelegate &&other) noexcept = default;

    template<typename FunctionPrototype>
    SingleCastDelegate<FunctionPrototype> &SingleCastDelegate<FunctionPrototype>::operator=(SingleCastDelegate &&other) noexcept = default;

    template<typename FunctionPrototype>
    SingleCastDelegate<FunctionPrototype>::~SingleCastDelegate() = default;

    template<typename FunctionPrototype>
    template<typename RetType, typename ObjectType, typename... Args>
    void SingleCastDelegate<FunctionPrototype>::bind(RetType (ObjectType::*function)(Args...), ObjectType *object) {
        functionPointer = [object, function](Args &&... args) -> RetType {
            return (object->*function)(std::forward<Args>(args)...);
        };
    }

    template<typename FunctionPrototype>
    template<typename BindFunctionPrototype>
    void SingleCastDelegate<FunctionPrototype>::bind(BindFunctionPrototype &&function) {
        functionPointer = function;
    }

    template<typename FunctionPrototype>
    void SingleCastDelegate<FunctionPrototype>::unbind() {
        functionPointer = nullptr;
    }

    template<typename FunctionPrototype>
    bool SingleCastDelegate<FunctionPrototype>::isBound() const {
        return functionPointer != nullptr;
    }

    template<typename FunctionPrototype>
    SingleCastDelegate<FunctionPrototype>::operator bool() const {
        return isBound();
    }

    template<typename FunctionPrototype>
    template<typename... Args>
    auto SingleCastDelegate<FunctionPrototype>::broadcast(Args &&... args) const {
        return functionPointer(std::forward<Args>(args)...);
    }
}