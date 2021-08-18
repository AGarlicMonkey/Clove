#include "Clove/Audio/OpenAL/OpenAlFactory.hpp"

#include "Clove/Audio/OpenAL/OpenAlBuffer.hpp"
#include "Clove/Audio/OpenAL/OpenAlError.hpp"
#include "Clove/Audio/OpenAL/OpenAlListener.hpp"
#include "Clove/Audio/OpenAL/OpenAlSource.hpp"

namespace clove {
    OpenAlFactory::OpenAlFactory() = default;

    OpenAlFactory::OpenAlFactory(OpenAlFactory &&other) noexcept = default;

    OpenAlFactory &OpenAlFactory::operator=(OpenAlFactory &&other) noexcept = default;

    OpenAlFactory::~OpenAlFactory() = default;

    Expected<std::unique_ptr<AhaBuffer>, std::runtime_error> OpenAlFactory::createAudioBuffer(AhaBuffer::Descriptor descriptor) noexcept {
        ALuint buffer{ 0 };
        alGenBuffers(1, &buffer);

        ALenum const error{ alGetError() };
        if(error != AL_NO_ERROR) {
            printErrorAl(error, __FILE__, __LINE__);
            return Unexpected{ std::runtime_error{ "Failed to create AhaBuffer. See log for details." } };
        }

        return std::unique_ptr<AhaBuffer>{ std::make_unique<OpenAlBuffer>(buffer, descriptor) };
    }

    Expected<std::unique_ptr<AhaSource>, std::runtime_error> OpenAlFactory::createAudioSource() noexcept {
        ALuint source{ 0 };
        alGenSources(1, &source);

        ALenum const error{ alGetError() };
        if(error != AL_NO_ERROR){
            printErrorAl(error, __FILE__, __LINE__);
            return Unexpected{ std::runtime_error{ "Failed to create AhaSource. See log for details." } };
        }

        return std::unique_ptr<AhaSource>{ std::make_unique<OpenAlSource>(source) };
    }

    Expected<std::unique_ptr<AhaListener>, std::runtime_error> OpenAlFactory::createAudioListener() noexcept {
        return std::unique_ptr<AhaListener>{ std::make_unique<OpenAlListener>() };
    }
}
