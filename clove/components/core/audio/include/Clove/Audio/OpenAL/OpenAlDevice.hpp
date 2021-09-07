#pragma once

#include "Clove/Audio/AhaDevice.hpp"

#include <AL/alc.h>

namespace clove {
    class OpenAlFactory;
}

namespace clove {
    /**
     * @brief Represents the actual hardware used for audio rendering. Can be quieried for info about the device.
     */
    class OpenAlDevice : public AhaDevice {
        //VARIABLES
    private:
        ALCdevice *alDevice{ nullptr };
        ALCcontext *alContext{ nullptr };

        std::unique_ptr<OpenAlFactory> factory;

        //FUNCTIONS
    public:
        OpenAlDevice() = delete;
        OpenAlDevice(ALCdevice *alDevice);

        OpenAlDevice(OpenAlDevice const &other) = delete;
        OpenAlDevice(OpenAlDevice &&other) noexcept;

        OpenAlDevice &operator=(OpenAlDevice const &other) = delete;
        OpenAlDevice &operator=(OpenAlDevice &&other) noexcept;

        ~OpenAlDevice();

        AhaFactory *getAudioFactory() const override;
    };
}