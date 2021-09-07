#include "Clove/Audio/OpenAL/OpenAlAha.hpp"

#include "Clove/Audio/OpenAL/OpenAlDevice.hpp"

#include <AL/alc.h>

namespace clove {
    Expected<std::unique_ptr<OpenAlDevice>, std::runtime_error> createOpenAlDevice() noexcept {
        ALCdevice *alDevice{ alcOpenDevice(nullptr) };
        if(alDevice == nullptr) {
            return Unexpected{ std::runtime_error{ "Failed to create OpenAL device" } };
        }

        return std::make_unique<OpenAlDevice>(alDevice);
    }
}