#pragma once

#include <cstdint>

namespace clove {
    /**
     * @brief Represents a globally unique identifier.
     * @details Guids can be copyable so there might be some with equal values but initialising 
     * a new Guid will result in a unique ID.
     */
    class Guid {
        //VARIABLES
    private:
        uint64_t guid{ 0 };

        //FUNCTIONS
    public:
        Guid();
        inline explicit Guid(uint64_t guid);

        inline Guid(Guid const &other);
        inline Guid(Guid &&other) noexcept;

        inline Guid &operator=(Guid const &other);
        inline Guid &operator=(Guid &&other) noexcept;

        inline ~Guid();

        inline uint64_t getId() const;

        inline operator uint64_t() const;
    };
}

#include "Guid.inl"