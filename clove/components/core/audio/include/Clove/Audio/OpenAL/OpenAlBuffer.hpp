#pragma once

#include "Clove/Audio/AhaBuffer.hpp"

#include <AL/al.h>

namespace clove {
    class OpenAlBuffer : public AhaBuffer {
        //VARIABLES
    private:
        ALuint buffer{ 0 };
        Descriptor descriptor;

        //FUNCTIONS
    public:
        OpenAlBuffer() = delete;
        OpenAlBuffer(ALuint buffer, Descriptor descriptor);

        OpenAlBuffer(OpenAlBuffer const &other) = delete;
        OpenAlBuffer(OpenAlBuffer &&other) noexcept;

        OpenAlBuffer &operator=(OpenAlBuffer const &other) = delete;
        OpenAlBuffer &operator=(OpenAlBuffer &&other) noexcept;

        ~OpenAlBuffer();

        void write(void const *data, size_t const size) override;

        ALuint getBufferId() const;
    };
}
