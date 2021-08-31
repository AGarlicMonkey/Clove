#include "Clove/Graphics/Validation/ValidationCommandBuffer.hpp"

namespace clove {
    namespace detail {
        template<typename QueueType, typename BufferType>
        void initialiseBuffer(QueueType *queue, BufferType *buffer) {
            bool const allowBufferReuse{ (queue->getDescriptor().flags & QueueFlags::ReuseBuffers) != 0 };
            dynamic_cast<ValidationCommandBuffer *>(buffer)->setAllowBufferReuse(allowBufferReuse);
        }

        template<typename SubmissionType>
        void validateBuffersUsage(SubmissionType const &submission) {
            for(auto &commandBuffer : submission.commandBuffers) {
                auto *buffer{ dynamic_cast<ValidationCommandBuffer *>(commandBuffer) };
                if(buffer->getCommandBufferUsage() == CommandBufferUsage::OneTimeSubmit && buffer->bufferHasBeenUsed()) {
                    CLOVE_ASSERT(false, "GraphicsCommandBuffer recorded with CommandBufferUsage::OneTimeSubmit has already been used. Only buffers recorded with CommandBufferUsage::Default can submitted multiples times after being recorded once.");
                    break;
                }
            }
        }

        template<typename SubmissionType>
        void markBuffersAsUsed(SubmissionType const &submission) {
            for(auto &commandBuffer : submission.commandBuffers) {
                dynamic_cast<ValidationCommandBuffer *>(commandBuffer)->markAsUsed();
            }
        }
    }

    //Graphics
    template<typename BaseQueueType>
    std::unique_ptr<GhaGraphicsCommandBuffer> ValidationGraphicsQueue<BaseQueueType>::allocateCommandBuffer() {
        auto commandBuffer{ BaseQueueType::allocateCommandBuffer() };

        detail::initialiseBuffer(this, commandBuffer.get());

        return commandBuffer;
    }

    template<typename BaseQueueType>
    void ValidationGraphicsQueue<BaseQueueType>::submit(GraphicsSubmitInfo const &submission, GhaFence *signalFence) {
        detail::validateBuffersUsage(submission);

        BaseQueueType::submit(submission, signalFence);

        detail::markBuffersAsUsed(submission);
    }

    //Compute
    template<typename BaseQueueType>
    std::unique_ptr<GhaComputeCommandBuffer> ValidationComputeQueue<BaseQueueType>::allocateCommandBuffer() {
        auto commandBuffer{ BaseQueueType::allocateCommandBuffer() };

        detail::initialiseBuffer(this, commandBuffer.get());

        return commandBuffer;
    }

    template<typename BaseQueueType>
    void ValidationComputeQueue<BaseQueueType>::submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) {
        detail::validateBuffersUsage(submission);

        BaseQueueType::submit(submission, signalFence);

        detail::markBuffersAsUsed(submission);
    }

    //Transfer
    template<typename BaseQueueType>
    std::unique_ptr<GhaTransferCommandBuffer> ValidationTransferQueue<BaseQueueType>::allocateCommandBuffer() {
        auto commandBuffer{ BaseQueueType::allocateCommandBuffer() };

        detail::initialiseBuffer(this, commandBuffer.get());

        return commandBuffer;
    }

    template<typename BaseQueueType>
    void ValidationTransferQueue<BaseQueueType>::submit(TransferSubmitInfo const &submission, GhaFence *signalFence) {
        detail::validateBuffersUsage(submission);

        BaseQueueType::submit(submission, signalFence);

        detail::markBuffersAsUsed(submission);
    }
}
