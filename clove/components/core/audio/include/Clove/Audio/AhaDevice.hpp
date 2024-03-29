#pragma once

#include <memory>

namespace clove {
    class AhaFactory;
}

namespace clove {
    /**
     * @brief Represents the actual hardware used for audio rendering. Can be quieried for info about the device.
     */
    class AhaDevice {
        //FUNCTIONS
    public:
        virtual ~AhaDevice() = default;

        virtual AhaFactory *getAudioFactory() const = 0;
    };
}