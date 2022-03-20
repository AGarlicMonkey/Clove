#pragma once

#include "Clove/Graphics/GhaTransferQueue.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
    class MetalTransferQueue : public GhaTransferQueue {
        //VARIABLES
    private:
        id<MTLCommandQueue> commandQueue;

        //FUNCTIONS
    public:
        MetalTransferQueue() = delete;
        MetalTransferQueue(id<MTLCommandQueue> commandQueue);

        MetalTransferQueue(MetalTransferQueue const &other) = delete;
        MetalTransferQueue(MetalTransferQueue &&other) noexcept;

        MetalTransferQueue &operator=(MetalTransferQueue const &other) = delete;
        MetalTransferQueue &operator=(MetalTransferQueue &&other) noexcept;

        ~MetalTransferQueue();

        std::unique_ptr<GhaTransferCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(std::unique_ptr<GhaTransferCommandBuffer> &buffer) override;

        void submit(TransferSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
