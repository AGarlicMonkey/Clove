#pragma once

namespace clove {
    /**
     * @brief A GhaFence is a synchronisation object that can be used to synchronise the application
     * with a graphics operation.
     */
    class GhaFence {
        //TYPES
    public:
        struct Descriptor {
            bool signaled{ false }; /**< If true, will initialise the GhaFence as signaled. */
        };

        //FUNCTIONS
    public:
        virtual ~GhaFence() = default;

        /**
         * @brief Makes the current thread wait until the GhaFence has been signaled.
         */
        virtual void wait() = 0;
        /**
         * @brief Resets the GhaFence back into an unsignaled state.
         */
        virtual void reset() = 0;
    };
}