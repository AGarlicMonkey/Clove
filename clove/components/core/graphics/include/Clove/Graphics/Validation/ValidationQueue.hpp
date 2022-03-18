#pragma once

#include "Clove/Graphics/GhaGraphicsQueue.hpp"
#include "Clove/Graphics/GhaComputeQueue.hpp"
#include "Clove/Graphics/GhaTransferQueue.hpp"

namespace clove {
    template<typename BaseQueueType>
    class ValidationGraphicsQueue : public BaseQueueType {
        //FUNCTIONS
    public:
        using BaseQueueType::BaseQueueType;

        void submit(GraphicsSubmitInfo const &submission, GhaFence *signalFence) override;
    };

    template<typename BaseQueueType>
    class ValidationComputeQueue : public BaseQueueType {
        //FUNCTIONS
    public:
        using BaseQueueType::BaseQueueType;

        void submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) override;
    };

    template<typename BaseQueueType>
    class ValidationTransferQueue : public BaseQueueType {
        //FUNCTIONS
    public:
        using BaseQueueType::BaseQueueType;

        void submit(TransferSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}

#include "ValidationQueue.inl"