#pragma once

#include "Clove/Delegate/DelegateHandle.hpp"

#include <functional>
#include <memory>
#include <unordered_map>

namespace clove {
    /**
	 * @brief A callback that can have multiple bindings.
	 * @tparam FunctionPrototype The function prototypes that can be bound to this delegate.
	 */
    template<typename FunctionPrototype>
    class MultiCastDelegate {
        //VARIABLES
    private:
        inline static DelegateHandle::IdType nextId{ 0 };
        std::unordered_map<DelegateHandle::IdType, std::function<FunctionPrototype>> functionPointers;

        std::shared_ptr<DelegateHandle::Proxy> handleProxy;

        //FUNCTIONS
    public:
        MultiCastDelegate();

        MultiCastDelegate(MultiCastDelegate const &other) = delete;
        MultiCastDelegate(MultiCastDelegate &&other) noexcept;

        MultiCastDelegate &operator=(MultiCastDelegate const &other) = delete;
        MultiCastDelegate &operator=(MultiCastDelegate &&other) noexcept;

        ~MultiCastDelegate();

        /**
         * @brief Bind a member function to this delegate.
		 * @param function The member function, i.e. &MyClass::foo.
		 * @param object The instance of the class to bind to.
		 * @return DelegateHandle The ID / scope of the binding.
		 */
        template<typename RetType, typename ObjectType, typename... Args>
        [[nodiscard]] DelegateHandle bind(RetType (ObjectType::*function)(Args...), ObjectType *object);
        /**
		 * @brief Bind a function to this delegate.
		 * @param function A lambda or free function.
		 * @return DelegateHandle The ID / scope of the binding.
		 */
        template<typename BindFunctionPrototype>
        [[nodiscard]] DelegateHandle bind(BindFunctionPrototype &&function);

        void unbind(DelegateHandle &handle);
        void unbindAll();

        /**
		 * @brief Broadcast to all listeners of this delegate.
		 * @param args Arguments to forward to the listeners.
		 */
        template<typename... Args>
        void broadcast(Args &&... args) const;
    };
}

#include "MultiCastDelegate.inl"