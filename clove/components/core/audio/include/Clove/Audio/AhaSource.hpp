#pragma once

#include <Clove/Maths/Vector.hpp>
#include <memory>
#include <vector>

namespace clove {
    class AhaBuffer;
}

namespace clove {
    /**
	 * @brief An AhaSource allows sound to played from a cetain point in space
	 */
    class AhaSource {
        //FUNCTIONS
    public:
        virtual ~AhaSource() = default;

        /**
		 * @brief Sets buffer to play audio from
		 */
        virtual void setBuffer(std::shared_ptr<AhaBuffer> buffer) = 0;

        /**
		 * @brief Queues buffers to be played sequentially.
		 * @details Buffers are appended to the end of the current queue.
		 * @param buffers Vector of buffers to add to the queue.
		 */
        virtual void queueBuffers(std::vector<std::shared_ptr<AhaBuffer>> buffers) = 0;
        /**
		 * @brief Removes processed buffers from the queue.
		 * @details Processed buffers can be removed from the queue, allowing their data 
         * to be changed and then added back in. Useful when double or triple buffering
         * to stream large audio files.
		 * @param numToQueue The number of buffers to remove from the queue. must be <= getNumBuffersProcessed.
		 * @returns A vector of AudioBuffers removed from the queue.
		 */
        virtual std::vector<std::shared_ptr<AhaBuffer>> unQueueBuffers(uint32_t const numToUnqueue) = 0;

        virtual void setPitch(float pitch)      = 0;
        virtual void setLooping(bool isLooping) = 0;

        virtual void setPosition(vec3f const &position) = 0;
        virtual void setVelocity(vec3f const &velocity) = 0;

        virtual vec3f getPosition() const = 0;
        virtual vec3f getVelocity() const = 0;

        /**
		 * @brief How many buffers are in the queue (not played or playing)
		 */
        virtual uint32_t getNumBuffersQueued() const = 0;
        /**
		 * @brief How many buffers have been played
		 */
        virtual uint32_t getNumBuffersProcessed() const = 0;

        virtual void play()   = 0;
        virtual void pause()  = 0;
        virtual void stop()   = 0;
        virtual void rewind() = 0;
    };
}
