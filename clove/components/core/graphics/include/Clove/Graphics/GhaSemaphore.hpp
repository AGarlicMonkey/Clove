#pragma once

namespace clove {
    /**
     * @brief A GhaSemaphore is a synchronisation object that is used to synchronise queues between each other.
     * @details Semaphores have two states: signaled and unsignaled. Once a signaled semaphore has been read it's
     * safe to assume that it has been reset back to unsignaled. This should be kept in mind when using a semaphore
     * to synchronise many queue submissions.
     */
    class GhaSemaphore {
        //FUNCTIONS
    public:
        virtual ~GhaSemaphore() = default;
    };
}