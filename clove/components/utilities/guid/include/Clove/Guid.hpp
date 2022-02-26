#pragma once

#include <cstdint>

namespace clove {
    /**
     * @brief Represents a globally unique identifier.
     * @details Guids can be copyable so there might be some with equal values but initialising 
     * a new Guid will result in a unique ID.
     */
    class Guid {
        //TYPES
    public:
        using Type = uint64_t;

        //VARIABLES
    private:
        Type guid{ 0 };

        //FUNCTIONS
    public:
        Guid();
        inline explicit Guid(Type guid);

        inline Guid(Guid const &other);
        inline Guid(Guid &&other) noexcept;

        inline Guid &operator=(Guid const &other);
        inline Guid &operator=(Guid &&other) noexcept;

        inline ~Guid();

        inline Type getId() const;

        inline operator Type() const;
    };
}

#include "Guid.inl"