#include <utility>

#pragma once

namespace garlic::clove {
    /**
     * @brief Allows Expected to be constructed as an error properly 
     * if Expected's T and E are the same type.
     * @tparam E The error type
     */
    template<typename E>
    class Unexpected {
        //VARIABLES
    public:
        E value;

        //FUNCTIONS
    public:
        Unexpected() noexcept = delete;
        Unexpected(E const &value) noexcept
            : value{ value } {
        }
        Unexpected(E &&value) noexcept
            : value(std::move(value)) {
        }

        Unexpected(Unexpected const &other) noexcept = default;
        Unexpected(Unexpected &&other) noexcept      = default;

        Unexpected &operator=(Unexpected const &other) noexcept = default;
        Unexpected &operator=(Unexpected &&other) noexcept = default;

        ~Unexpected() noexcept = default;
    };

    /**
     * @brief Wraps either a value of type T or an error of type E.
     * @details Expected is used to return from functions either an
     * expected value or an error if the function fails.
     * @tparam T The type of the expected value.
     * @tparam E The type of the error. For example, can be an error
     * code or an exception.
     */
    template<typename T, typename E>
    class Expected {
        //VARIABLES
    private:
        union {
            T value;
            E error;
        };
        bool ok{ true };

        //FUNCTIONS
    public:
        Expected() noexcept = default;
        Expected(T const &value) noexcept;
        Expected(T &&value) noexcept;
        Expected(Unexpected<E> const &error) noexcept;
        Expected(Unexpected<E> &&error) noexcept;

        Expected(Expected const &other) noexcept = default;
        Expected(Expected &&other) noexcept      = default;

        Expected &operator=(Expected const &other) noexcept = default;
        Expected &operator=(Expected &&other) noexcept = default;

        ~Expected() noexcept {}

        bool hasValue() const noexcept;
        operator bool() const noexcept;

        T &getValue() &;
        T const &getValue() const &;
        T &&getValue() &&;
        T const &&getValue() const &&;

        T *operator->();
        T const *operator->() const;

        T &operator*() &;
        T const &operator*() const &;
        T &&operator*() &&;
        T const &&operator*() const &&;

        E &getError() &;
        E const &getError() const &;
        E &&getError() &&;
        E const &&getError() const &&;
    };
}

#include "Expected.inl"