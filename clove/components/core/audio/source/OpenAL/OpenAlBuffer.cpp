#include "Clove/Audio/OpenAL/OpenAlBuffer.hpp"

#include "Clove/Audio/OpenAL/OpenAlError.hpp"

#include <utility>

namespace clove {
    namespace {
        ALenum convertFormat(AhaBuffer::Format format) {
            switch(format) {
                case AhaBuffer::Format::Mono8:
                    return AL_FORMAT_MONO8;
                case AhaBuffer::Format::Mono16:
                    return AL_FORMAT_MONO16;
                case AhaBuffer::Format::Stereo8:
                    return AL_FORMAT_STEREO8;
                case AhaBuffer::Format::Stereo16:
                    return AL_FORMAT_STEREO16;
            }
        }
    }

    OpenAlBuffer::OpenAlBuffer(ALuint buffer, Descriptor descriptor)
        : buffer{ buffer }
        , descriptor{ descriptor } {
    }

    OpenAlBuffer::OpenAlBuffer(OpenAlBuffer &&other) noexcept = default;

    OpenAlBuffer &OpenAlBuffer::operator=(OpenAlBuffer &&other) noexcept = default;

    OpenAlBuffer::~OpenAlBuffer() {
        alCall(alDeleteBuffers(1, &buffer));
    }

    void OpenAlBuffer::write(void const *data, size_t const size) {
        alCall(alBufferData(buffer, convertFormat(descriptor.format), data, size, descriptor.sampleRate));
    }

    ALuint OpenAlBuffer::getBufferId() const {
        return buffer;
    }
}
