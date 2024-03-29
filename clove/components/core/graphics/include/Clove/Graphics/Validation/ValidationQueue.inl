#include "Clove/Graphics/Validation/ValidationCommandBuffer.hpp"

namespace clove {
    namespace internal {
        template<typename SubmissionType>
        void validateBuffersUsage(SubmissionType const &submission) {
            for(auto &commandBuffer : submission.commandBuffers) {
                auto *buffer{ dynamic_cast<ValidationCommandBuffer *>(commandBuffer) };
                CLOVE_ASSERT_MSG(!buffer->hasBufferBeenSubmitted(), "Command buffer has already been submitted used. Command buffers can only be recorded to and submitted for work once.");
            }
        }

        template<typename SubmissionType>
        void markBuffersAsUsed(SubmissionType const &submission) {
            for(auto &commandBuffer : submission.commandBuffers) {
                dynamic_cast<ValidationCommandBuffer *>(commandBuffer)->markAsSubmitted();
            }
        }
    }

    //Graphics
    template<typename BaseQueueType>
    void ValidationGraphicsQueue<BaseQueueType>::submit(GraphicsSubmitInfo const &submission, GhaFence *signalFence) {
        internal::validateBuffersUsage(submission);
        BaseQueueType::submit(submission, signalFence);
        internal::markBuffersAsUsed(submission);
    }

    //Compute
    template<typename BaseQueueType>
    void ValidationComputeQueue<BaseQueueType>::submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) {
        internal::validateBuffersUsage(submission);
        BaseQueueType::submit(submission, signalFence);
        internal::markBuffersAsUsed(submission);
    }

    //Transfer
    template<typename BaseQueueType>
    void ValidationTransferQueue<BaseQueueType>::submit(TransferSubmitInfo const &submission, GhaFence *signalFence) {
        internal::validateBuffersUsage(submission);
        BaseQueueType::submit(submission, signalFence);
        internal::markBuffersAsUsed(submission);
    }
}
