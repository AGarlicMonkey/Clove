#pragma once

#include <cstddef>

namespace clove {
	/**
	 * @brief Represents a buffer of data that can be played with an AhaSource
	 * @details Created from an AhaFactory. Can be created empty and then
	 * updated later.
	 */
    class AhaBuffer {
		//TYPES
	public:
		enum class Format {
			Mono8,
			Mono16,
			Stereo8,
			Stereo16
		};

		struct Descriptor{
            Format format;
            size_t sampleRate{ 0 };
        };

        //FUNCTIONS
    public:
        virtual ~AhaBuffer() = default;

        /**
         * @brief Write data into the buffer.
         * @param data A pointer to the data that will be written to this buffer.
         * @param size Size of the region to write to.
         */
        virtual void write(void const *data, size_t const size) = 0;
    };
}
