#pragma once

#include "Clove/Graphics/GhaComputeQueue.hpp"
#include "Clove/Graphics/Queue.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
    class MetalComputeQueue : public GhaComputeQueue {
        //VARIABLES
    private:
        CommandQueueDescriptor descriptor{};
        id<MTLCommandQueue> commandQueue{ nullptr };

        bool allowBufferReuse{ false };

        //FUNCTIONS
    public:
        MetalComputeQueue() = delete;
        MetalComputeQueue(CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue);

        MetalComputeQueue(MetalComputeQueue const &other) = delete;
        MetalComputeQueue(MetalComputeQueue &&other) noexcept;

        MetalComputeQueue &operator=(MetalComputeQueue const &other) = delete;
        MetalComputeQueue &operator=(MetalComputeQueue &&other) noexcept;

        ~MetalComputeQueue();

        CommandQueueDescriptor const &getDescriptor() const override;

        std::unique_ptr<GhaComputeCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(GhaComputeCommandBuffer &buffer) override;

        void submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
