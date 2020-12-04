#pragma once

#include <memory>

namespace garlic::clove {
    class AudioFactory;
}

namespace garlic::clove {
    /**
     * @brief Represents the actual hardware used for audio rendering. Can be quieried for info about the device.
     */
    class AudioDevice {
        //FUNCTIONS
    public:
        virtual ~AudioDevice() = default;

        virtual std::shared_ptr<AudioFactory> getAudioFactory() const = 0;
    };
}