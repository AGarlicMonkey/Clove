#pragma once

namespace clove {
    class GhaFactory;
}

namespace clove {
    /**
     * @brief Represents the actual hardware used for rendering. Can be quieried for info about the device.
     */
    class GhaDevice {
        //TYPES
    public:
        struct Limits {
            size_t minUniformBufferOffsetAlignment{ 0 };
        };

        //FUNCTIONS
    public:
        virtual ~GhaDevice() = default;

        /**
         * @brief Returns a pointer to the factory object. The lifetime of the 
         * factory is tied to the lifetime of this device.
         * @return 
         */
        virtual GhaFactory *getGraphicsFactory() const = 0;

        /**
         * @brief Stalls the current thread until the device is idle.
         */
        virtual void waitForIdleDevice() = 0;

        virtual Limits getLimits() const = 0;
    };
}