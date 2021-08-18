#pragma once

#include <memory>

namespace clove{
    class GhaFactory;
}

namespace clove {
    /**
     * @brief Represents the actual hardware used for rendering. Can be quieried for info about the device.
     */
    class GhaDevice {
        //TYPES
    public:
        struct Limits{
            size_t minUniformBufferOffsetAlignment{ 0 };
        };

        //FUNCTIONS
    public:
        virtual ~GhaDevice() = default;

        virtual std::shared_ptr<GhaFactory> getGraphicsFactory() const = 0;

        /**
         * @brief Stalls the current thread until the device is idle.
         */
        virtual void waitForIdleDevice() = 0;

        virtual Limits getLimits() const = 0;
    };
}