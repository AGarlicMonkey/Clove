#include "Clove/Audio/OpenAL/OpenAlDevice.hpp"

#include "Clove/Audio/OpenAL/OpenAlFactory.hpp"
#include "Clove/Audio/OpenAL/OpenAlLog.hpp"

#include <AL/al.h>

namespace clove {
    namespace {
        void printErrorAlc(ALenum error, std::string_view const fileName, uint_fast32_t const line) {
            switch(error) {
                case ALC_INVALID_VALUE:
                    CLOVE_LOG(OpenAl, LogLevel::Error, "ALC_INVALID_VALUE: An invalid value was passed to an OpenAL function. File: {0} Line {1}", fileName, line);
                    break;
                case ALC_INVALID_DEVICE:
                    CLOVE_LOG(OpenAl, LogLevel::Error, "ALC_INVALID_DEVICE: A bad device was passed to an OpenAL function. File: {0} Line {1}", fileName, line);
                    break;
                case ALC_INVALID_CONTEXT:
                    CLOVE_LOG(OpenAl, LogLevel::Error, "ALC_INVALID_CONTEXT: A bad context was passed to an OpenAL function. File: {0} Line {1}", fileName, line);
                    break;
                case ALC_INVALID_ENUM:
                    CLOVE_LOG(OpenAl, LogLevel::Error, "ALC_INVALID_ENUM: An unkown enum was passed to an OpenAL function. File: {0} Line {1}", fileName, line);
                    break;
                case ALC_OUT_OF_MEMORY:
                    CLOVE_LOG(OpenAl, LogLevel::Error, "ALC_OUT_OF_MEMORY: The requested operation caused OpenAL to run out of memory. File: {0} Line {1}", fileName, line);
                    break;
            }
        }
    }

#define alcCall(call, device)                         \
    call;                                             \
    {                                                 \
        ALenum error{ alcGetError(device) };          \
        if(error != AL_NO_ERROR) {                    \
            printErrorAlc(error, __FILE__, __LINE__); \
        }                                             \
    }

    OpenAlDevice::OpenAlDevice(ALCdevice *alDevice)
        : alDevice{ alDevice }
        , factory{ std::make_unique<OpenAlFactory>() } {
        alDevice = alcOpenDevice(nullptr);
        if(alDevice == nullptr) {
            CLOVE_LOG(CloveAhaOpenAl, LogLevel::Error, "Failed to create OpenAL device");
            return;
        }

        alcCall(alContext = alcCreateContext(alDevice, nullptr), alDevice);
        alcCall(alcMakeContextCurrent(alContext), alDevice);
    }

    OpenAlDevice::OpenAlDevice(OpenAlDevice &&other) noexcept = default;

    OpenAlDevice &OpenAlDevice::operator=(OpenAlDevice &&other) noexcept = default;

    OpenAlDevice::~OpenAlDevice() {
        alcCall(alcMakeContextCurrent(nullptr), alDevice);
        alcCall(alcDestroyContext(alContext), alDevice);

        alcCloseDevice(alDevice);
    }

    AhaFactory *OpenAlDevice::getAudioFactory() const {
        return factory.get();
    }
}