#pragma once

#include "Clove/Graphics/GhaTransferQueue.hpp"
#include "Clove/Graphics/Queue.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
    class MetalTransferQueue : public GhaTransferQueue {
        //VARIABLES
    private:
        CommandQueueDescriptor descriptor{};
        id<MTLCommandQueue> commandQueue;

        bool allowBufferReuse{ false };

        //FUNCTIONS
    public:
        MetalTransferQueue() = delete;
        MetalTransferQueue(CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue);

        MetalTransferQueue(MetalTransferQueue const &other) = delete;
        MetalTransferQueue(MetalTransferQueue &&other) noexcept;

        MetalTransferQueue &operator=(MetalTransferQueue const &other) = delete;
        MetalTransferQueue &operator=(MetalTransferQueue &&other) noexcept;

        ~MetalTransferQueue();

        CommandQueueDescriptor const &getDescriptor() const override;

        std::unique_ptr<GhaTransferCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(GhaTransferCommandBuffer &buffer) override;

        void submit(TransferSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
