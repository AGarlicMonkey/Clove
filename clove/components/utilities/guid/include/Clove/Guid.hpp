#pragma once

#include <cstdint>

namespace clove {
    class Guid {
        //VARIABLES
    private:
        uint64_t guid{ 0 };

        //FUNCTIONS
    public:
        Guid();

        Guid(Guid const &other) = delete;
        inline Guid(Guid &&other) noexcept;

        Guid &operator=(Guid const &other) = delete;
        inline Guid &operator =(Guid &&other) noexcept;

        inline ~Guid();

        inline uint64_t getId() const;

        inline operator uint64_t() const;
    };
}

#include "Guid.inl"