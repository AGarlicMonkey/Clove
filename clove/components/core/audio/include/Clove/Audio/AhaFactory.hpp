#pragma once

#include "Clove/Audio/AhaBuffer.hpp"
#include "Clove/Audio/AhaListener.hpp"
#include "Clove/Audio/AhaSource.hpp"

#include <Clove/Expected.hpp>
#include <stdexcept>

namespace garlic::clove {
    /**
	 * @brief Responsible for creating all audio objects.
	 */
    class AhaFactory {
        //FUNCTIONS
    public:
        virtual ~AhaFactory() = default;

        virtual Expected<std::unique_ptr<AhaBuffer>, std::runtime_error> createAudioBuffer(AhaBuffer::Descriptor descriptor) = 0;
        virtual Expected<std::unique_ptr<AhaSource>, std::runtime_error> createAudioSource()                                 = 0;
        virtual Expected<std::unique_ptr<AhaListener>, std::runtime_error> createAudioListener()                             = 0;
    };
}
