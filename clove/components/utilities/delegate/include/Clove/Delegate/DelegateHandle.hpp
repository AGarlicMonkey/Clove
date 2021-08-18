#pragma once

#include <cinttypes>
#include <functional>
#include <memory>
#include <optional>

namespace clove {
    /**
	 * @brief Defines the scope of a delegate binding.
	 * @details In it's simplest for the handle is just an ID to the
	 * binding inside the delegate. If this object goes out of scope or 
	 * is reset manually then the delegate binding associated with this 
	 * object is removed. 
	 * @see MultiCastDelegate
	 */
    class DelegateHandle {
        template<typename FunctionPrototype>
        friend class MultiCastDelegate;

        //TYPES
    private:
        using IdType = int32_t;

        struct Proxy {
            std::function<void(DelegateHandle &)> unbind;
        };

        //VARIABLES
    private:
        static constexpr IdType INVALID_ID{ -1 };
        std::optional<IdType> id{};

        std::weak_ptr<Proxy> proxy;

        //FUNCTIONS
    public:
        DelegateHandle();
        DelegateHandle(IdType id, std::weak_ptr<Proxy> proxy);

        DelegateHandle(DelegateHandle const &other) = delete;
        DelegateHandle(DelegateHandle &&other) noexcept;

        DelegateHandle &operator=(DelegateHandle const &other) = delete;
        DelegateHandle &operator=(DelegateHandle &&other) noexcept;

        ~DelegateHandle();

        bool isValid() const;

        void reset();
    };
}