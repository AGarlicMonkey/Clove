#pragma once

#include "Clove/Graphics/GhaComputeQueue.hpp"

#include <MetalKit/MetalKit.h>

namespace clove {
    class MetalComputeQueue : public GhaComputeQueue {
        //VARIABLES
    private:
        id<MTLCommandQueue> commandQueue{ nullptr };

        //FUNCTIONS
    public:
        MetalComputeQueue() = delete;
        MetalComputeQueue(id<MTLCommandQueue> commandQueue);

        MetalComputeQueue(MetalComputeQueue const &other) = delete;
        MetalComputeQueue(MetalComputeQueue &&other) noexcept;

        MetalComputeQueue &operator=(MetalComputeQueue const &other) = delete;
        MetalComputeQueue &operator=(MetalComputeQueue &&other) noexcept;

        ~MetalComputeQueue();

        std::unique_ptr<GhaComputeCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(std::unique_ptr<GhaComputeCommandBuffer> &buffer) override;

        void submit(ComputeSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
