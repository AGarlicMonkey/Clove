#pragma once

#include "Clove/Graphics/GhaTransferCommandBuffer.hpp"
#include "Clove/Graphics/PipelineObject.hpp"
#include "Clove/Graphics/Queue.hpp"

#include <vector>

namespace clove {
    class GhaFence;
    class GhaSemaphore;

    struct TransferSubmitInfo {
        std::vector<std::pair<GhaSemaphore const *, PipelineStage>> waitSemaphores; /**< What semaphores to wait on at what stage */
        std::vector<GhaTransferCommandBuffer *> commandBuffers;                     /**< The command buffers to execute */
        std::vector<GhaSemaphore const *> signalSemaphores;                         /**< The semaphores that will be signaled when completed */
    };
}

namespace clove {
    /**
     * @brief Creates buffers that can record transfer commands and then be submitted.
     */
    class GhaTransferQueue {
        //FUNCTIONS
    public:
        virtual ~GhaTransferQueue() = default;

        virtual CommandQueueDescriptor const &getDescriptor() const = 0;

        virtual std::unique_ptr<GhaTransferCommandBuffer> allocateCommandBuffer()         = 0;
        virtual void freeCommandBuffer(std::unique_ptr<GhaTransferCommandBuffer> &buffer) = 0;

        /**
         * @brief Submit command buffers to be processed.
         * @details All buffers in a single submission will start in order but will likely finish out of order.
         * Batch them together like this if they can run at the same time. Each call to this submit function
         * will need to wait on previous submissions.
         * @param submission
         * @param signalFence An optional fence that will be signaled when the submission is complete.
         */
        virtual void submit(TransferSubmitInfo const &submission, GhaFence *signalFence) = 0;
    };
}
