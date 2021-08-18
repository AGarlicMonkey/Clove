#pragma once

#include <functional>

namespace clove {
    /**
	 * @brief A callback that can only have a single binding.
	 * @tparam FunctionPrototype The function prototypes that can be bound to this delegate.
	 */
    template<typename FunctionPrototype>
    class SingleCastDelegate {
        //VARIABLES
    private:
        std::function<FunctionPrototype> functionPointer;

        //FUNCTIONS
    public:
        SingleCastDelegate();

        SingleCastDelegate(SingleCastDelegate const &other) = delete;
        SingleCastDelegate(SingleCastDelegate &&other) noexcept;

        SingleCastDelegate &operator=(SingleCastDelegate const &other) = delete;
        SingleCastDelegate &operator=(SingleCastDelegate &&other) noexcept;

        ~SingleCastDelegate();

        /**
		 * @brief Bind a member function to this delegate.
		 * @param function The member function, i.e. &MyClass::foo.
		 * @param object The instance of the class to bind to.
		 */
        template<typename RetType, typename ObjectType, typename... Args>
        void bind(RetType (ObjectType::*function)(Args...), ObjectType *object);
        /**
		 * @brief Bind a function to this delegate.
		 * @param function A lambda or free function.
		 */
        template<typename BindFunctionPrototype>
        void bind(BindFunctionPrototype &&function);

        void unbind();

        bool isBound() const;
        operator bool() const;

        /**
		 * @brief Broadcast to the listener of this delegate.
		 * @param args Arguments to forward to the listener.
		 */
        template<typename... Args>
        auto broadcast(Args &&... args) const;
    };
}

#include "SingleCastDelegate.inl"