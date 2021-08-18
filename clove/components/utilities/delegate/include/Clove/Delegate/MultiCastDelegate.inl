namespace clove {
    template<typename FunctionPrototype>
    MultiCastDelegate<FunctionPrototype>::MultiCastDelegate() {
        handleProxy         = std::make_shared<DelegateHandle::Proxy>();
        handleProxy->unbind = [this](DelegateHandle &handle) {
            unbind(handle);
        };
    }

    template<typename FunctionPrototype>
    MultiCastDelegate<FunctionPrototype>::MultiCastDelegate(MultiCastDelegate &&other) noexcept {
        functionPointers = std::move(other.functionPointers);
    }

    template<typename FunctionPrototype>
    MultiCastDelegate<FunctionPrototype> &MultiCastDelegate<FunctionPrototype>::operator=(MultiCastDelegate &&other) noexcept {
        functionPointers = std::move(other.functionPointers);
        return *this;
    }

    template<typename FunctionPrototype>
    MultiCastDelegate<FunctionPrototype>::~MultiCastDelegate() = default;

    template<typename FunctionPrototype>
    template<typename RetType, typename ObjectType, typename... Args>
    DelegateHandle MultiCastDelegate<FunctionPrototype>::bind(RetType (ObjectType::*function)(Args...), ObjectType *object) {
        auto functionPointer = [object, function](Args &&... args) -> RetType {
            return (object->*function)(std::forward<Args>(args)...);
        };

        auto const id{ nextId++ };
        DelegateHandle handle{ id, handleProxy };
        functionPointers.emplace(std::make_pair(id, functionPointer));

        return handle;
    }

    template<typename FunctionPrototype>
    template<typename BindFunctionPrototype>
    DelegateHandle MultiCastDelegate<FunctionPrototype>::bind(BindFunctionPrototype &&function) {
        auto const id{ nextId++ };
        DelegateHandle handle{ id, handleProxy };
        functionPointers.emplace(std::make_pair(id, function));

        return handle;
    }

    template<typename FunctionPrototype>
    void MultiCastDelegate<FunctionPrototype>::unbind(DelegateHandle &handle) {
        functionPointers.erase(handle.id.value_or(DelegateHandle::INVALID_ID));
        handle.id.reset();
    }

    template<typename FunctionPrototype>
    void MultiCastDelegate<FunctionPrototype>::unbindAll() {
        functionPointers.clear();
    }

    template<typename FunctionPrototype>
    template<typename... Args>
    void MultiCastDelegate<FunctionPrototype>::broadcast(Args &&... args) const {
        for(auto &[handle, function] : functionPointers) {
            if(function) {
                function(std::forward<Args>(args)...);
            }
        }
    }
}