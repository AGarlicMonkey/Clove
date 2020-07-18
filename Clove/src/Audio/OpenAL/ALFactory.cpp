#include "Clove/Audio/OpenAL/ALFactory.hpp"

#include "Clove/Audio/OpenAL/ALBuffer.hpp"
#include "Clove/Audio/OpenAL/ALError.hpp"
#include "Clove/Audio/OpenAL/ALListener.hpp"
#include "Clove/Audio/OpenAL/ALSource.hpp"

namespace clv {
    ALFactory::ALFactory() {
        alDevice = alcOpenDevice(nullptr);
        if(!alDevice) {
            GARLIC_LOG(garlicLogContext, Log::Level::Error, "Failed to create OpenAL device");
            return;
        }

        alcCall(alContext = alcCreateContext(alDevice, nullptr), alDevice);
        alcCall(alcMakeContextCurrent(alContext), alDevice);
    }

    ALFactory::ALFactory(ALFactory&& other) noexcept = default;

    ALFactory& ALFactory::operator=(ALFactory&& other) noexcept = default;

    ALFactory::~ALFactory() {
        alcCall(alcMakeContextCurrent(nullptr), alDevice);
        alcCall(alcDestroyContext(alContext), alDevice);

        alcCloseDevice(alDevice);
    }

    std::unique_ptr<AudioBuffer> ALFactory::createAudioBuffer() {
        return std::make_unique<ALBuffer>();
    }

    std::unique_ptr<AudioBuffer> ALFactory::createAudioBuffer(const AudioBuffer::DataInfo& info) {
        return std::make_unique<ALBuffer>(info);
    }

    std::unique_ptr<AudioSource> ALFactory::createAudioSource() {
        return std::make_unique<ALSource>();
    }

    std::unique_ptr<AudioListener> ALFactory::createAudioListener() {
        return std::make_unique<ALListener>();
    }
}