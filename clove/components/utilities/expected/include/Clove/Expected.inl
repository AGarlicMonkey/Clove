#include <cassert>

namespace garlic::clove {
    template<typename T, typename E>
    Expected<T, E>::Expected(T const &value) noexcept
        : value{ value } {
    }

    template<typename T, typename E>
    Expected<T, E>::Expected(T &&value) noexcept
        : value{ std::move(value) } {
    }

    template<typename T, typename E>
    Expected<T, E>::Expected(Unexpected<E> const &error) noexcept
        : error{ error.value }
        , ok{ false } {
    }

    template<typename T, typename E>
    Expected<T, E>::Expected(Unexpected<E> &&error) noexcept
        : error{ std::move(error.value) }
        , ok{ false } {
    }

    template<typename T, typename E>
    bool Expected<T, E>::hasValue() const noexcept {
        return ok;
    }

    template<typename T, typename E>
    Expected<T, E>::operator bool() const noexcept {
        return hasValue();
    }

    template<typename T, typename E>
    T &Expected<T, E>::getValue() & {
        if(!ok) {
            throw error;
        }
        return value;
    }

    template<typename T, typename E>
    T const &Expected<T, E>::getValue() const & {
        if(!ok) {
            throw error;
        }
        return value;
    }

    template<typename T, typename E>
    T &&Expected<T, E>::getValue() && {
        if(!ok) {
            throw error;
        }
        return std::move(value);
    }

    template<typename T, typename E>
    T const &&Expected<T, E>::getValue() const && {
        if(!ok) {
            throw error;
        }
        return std::move(value);
    }

    template<typename T, typename E>
    T *Expected<T, E>::operator->() {
        if(!ok) {
            throw error;
        }
        return &value;
    }

    template<typename T, typename E>
    T const *Expected<T, E>::operator->() const {
        if(!ok) {
            throw error;
        }
        return &value;
    }

    template<typename T, typename E>
    T &Expected<T, E>::operator*() & {
        return getValue();
    }

    template<typename T, typename E>
    T const &Expected<T, E>::operator*() const & {
        return getValue();
    }

    template<typename T, typename E>
    T &&Expected<T, E>::operator*() && {
        return std::move(getValue());
    }

    template<typename T, typename E>
    T const &&Expected<T, E>::operator*() const && {
        return std::move(getValue());
    }

    template<typename T, typename E>
    E &Expected<T, E>::getError() & {
        assert(!ok);
        return error;
    }

    template<typename T, typename E>
    E const &Expected<T, E>::getError() const & {
        assert(!ok);
        return error;
    }

    template<typename T, typename E>
    E &&Expected<T, E>::getError() && {
        assert(!ok);
        return std::move(error);
    }

    template<typename T, typename E>
    E const &&Expected<T, E>::getError() const && {
        assert(!ok);
        return std::move(error);
    }
}