#pragma once

#include "Clove/Graphics/GhaComputeCommandBuffer.hpp"
#include "Clove/Graphics/GhaGraphicsPipelineObject.hpp"
#include "Clove/Graphics/Queue.hpp"

#include <memory>
#include <vector>

namespace clove {
    class GhaFence;
    class GhaSemaphore;

    struct ComputeSubmitInfo {
        std::vector<std::pair<GhaSemaphore const *, PipelineStage>> waitSemaphores; /**< What semaphores to wait on at what stage */
        std::vector<GhaComputeCommandBuffer *> commandBuffers;                      /**< The command buffers to execute */
        std::vector<GhaSemaphore const *> signalSemaphores;                         /**< The semaphores that will be signaled when completed */
    };
}

namespace clove {
    /**
     * @brief Creates buffers that can be recorded for compute commands and then submitted.
     */
    class GhaComputeQueue {
        //FUNCTIONS
    public:
        virtual ~GhaComputeQueue() = default;

        virtual CommandQueueDescriptor const &getDescriptor() const = 0;

        virtual std::unique_ptr<GhaComputeCommandBuffer> allocateCommandBuffer()         = 0;
        virtual void freeCommandBuffer(std::unique_ptr<GhaComputeCommandBuffer> &buffer) = 0;

        /**
         * @brief Submit command buffers to be processed.
         * @details All buffers in a single submission will start in order but will likely finish out of order.
         * Batch them together like this if they can run at the same time. Each call to this submit function
         * will need to wait on previous submissions.
         * @param submission
         * @param signalFence An optional fence that will be signaled when the submission is complete.
         */
        virtual void submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) = 0;
    };
}
