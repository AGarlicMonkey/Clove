#pragma once

#include "Clove/Graphics/GhaComputeCommandBuffer.hpp"
#include "Clove/Graphics/GhaGraphicsPipelineObject.hpp"

#include <memory>
#include <vector>

namespace clove {
    class GhaFence;
    class GhaSemaphore;

    struct ComputeSubmitInfo {
        std::vector<std::pair<GhaSemaphore const *, PipelineStage>> waitSemaphores; /**< What semaphores to wait on at what stage */
        std::vector<GhaComputeCommandBuffer const *> commandBuffers;                /**< The command buffers to execute */
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

        virtual std::unique_ptr<GhaComputeCommandBuffer> allocateCommandBuffer() = 0;
        virtual void freeCommandBuffer(GhaComputeCommandBuffer &buffer)          = 0;

        /**
         * @brief Submit command buffers to be processed.
         * @details All buffers in a single submission will start in order but will likely finish out of order.
         * Batch them together like this if they can run at the same time. Each call to this submit function
         * will need to wait on previous submissions.
         * @param signalFence An optional fence that will be signaled when all submissions are complete.
         */
        virtual void submit(std::vector<ComputeSubmitInfo> const &submissions, GhaFence *signalFence) = 0;
    };
}
