#pragma once

#include "Clove/Graphics/GhaGraphicsQueue.hpp"
#include "Clove/Graphics/Queue.hpp"

#include <MetalKit/MetalKit.h>

@class MetalView;

namespace clove {
    class MetalGraphicsQueue : public GhaGraphicsQueue {
        //VARIABLES
    private:
        CommandQueueDescriptor descriptor{};

        id<MTLCommandQueue> commandQueue;

        //FUNCTIONS
    public:
        MetalGraphicsQueue() = delete;
        MetalGraphicsQueue(CommandQueueDescriptor descriptor, id<MTLCommandQueue> commandQueue);

        MetalGraphicsQueue(MetalGraphicsQueue const &other) = delete;
        MetalGraphicsQueue(MetalGraphicsQueue &&other) noexcept;

        MetalGraphicsQueue &operator=(MetalGraphicsQueue const &other) = delete;
        MetalGraphicsQueue &operator=(MetalGraphicsQueue &&other) noexcept;

        ~MetalGraphicsQueue();

        CommandQueueDescriptor const &getDescriptor() const override;

        std::unique_ptr<GhaGraphicsCommandBuffer> allocateCommandBuffer() override;
        void freeCommandBuffer(GhaGraphicsCommandBuffer &buffer) override;

        void submit(GraphicsSubmitInfo const &submission, GhaFence *signalFence) override;
    };
}
