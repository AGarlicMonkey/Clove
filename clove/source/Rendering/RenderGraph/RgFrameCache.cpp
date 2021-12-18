#include "Clove/Rendering/RenderGraph/RgFrameCache.hpp"

#include "Clove/Rendering/RenderGraph/CacheUtils.hpp"

#include <functional>

namespace clove {
    RgFrameCache::RgFrameCache(GhaFactory *ghaFactory, GhaGraphicsQueue *graphicsQueue, GhaComputeQueue *computeQueue, GhaComputeQueue *asyncComputeQueue, GhaTransferQueue *transferQueue)
        : ghaFactory{ ghaFactory }
        , graphicsQueue{ graphicsQueue }
        , computeQueue{ computeQueue }
        , asyncComputeQueue{ asyncComputeQueue }
        , transferQueue{ transferQueue } {
    }

    RgFrameCache::RgFrameCache(RgFrameCache &&other) noexcept = default;

    RgFrameCache &RgFrameCache::operator=(RgFrameCache &&other) noexcept = default;

    RgFrameCache::~RgFrameCache() = default;

    void RgFrameCache::reset() {
        bufferPool.reset();
        imagePool.reset();
        imageViewPool.reset();
        framebufferPool.reset();
        descriptorPoolPool.reset();
        semaphorePool.reset();

        for(auto &commandBuffer : allocatedGraphicsCommandBuffers) {
            graphicsQueue->freeCommandBuffer(commandBuffer);
        }
        allocatedGraphicsCommandBuffers.clear();

        for(auto &commandBuffer : allocatedComputeCommandBuffers) {
            computeQueue->freeCommandBuffer(commandBuffer);
        }
        allocatedComputeCommandBuffers.clear();

        for(auto &commandBuffer : allocatedAsyncComputeCommandBuffers) {
            asyncComputeQueue->freeCommandBuffer(commandBuffer);
        }
        allocatedAsyncComputeCommandBuffers.clear();

        for(auto &commandBuffer : allocatedTransferCommandBuffers) {
            transferQueue->freeCommandBuffer(commandBuffer);
        }
        allocatedTransferCommandBuffers.clear();
    }

    void RgFrameCache::clear() {
        reset();

        bufferPool.clear();
        imagePool.clear();
        imageViewPool.clear();
        framebufferPool.clear();
        descriptorPoolPool.clear();
        semaphorePool.clear();
    }

    GhaBuffer *RgFrameCache::allocateBuffer(GhaBuffer::Descriptor descriptor) {
        PoolId bufferId{ 0 };
        CacheUtils::hashCombine(bufferId, descriptor.size);
        CacheUtils::hashCombine(bufferId, descriptor.usageFlags);
        CacheUtils::hashCombine(bufferId, descriptor.sharingMode);
        CacheUtils::hashCombine(bufferId, descriptor.memoryType);

        return bufferPool.retrieve(bufferId, [&]() {
            return ghaFactory->createBuffer(descriptor).getValue();
        });
    }

    GhaImage *RgFrameCache::allocateImage(GhaImage::Descriptor descriptor) {
        PoolId imageId{ 0 };
        CacheUtils::hashCombine(imageId, descriptor.type);
        CacheUtils::hashCombine(imageId, descriptor.usageFlags);
        CacheUtils::hashCombine(imageId, descriptor.dimensions.x);
        CacheUtils::hashCombine(imageId, descriptor.dimensions.y);
        CacheUtils::hashCombine(imageId, descriptor.arrayCount);
        CacheUtils::hashCombine(imageId, descriptor.format);
        CacheUtils::hashCombine(imageId, descriptor.sharingMode);

        return imagePool.retrieve(imageId, [&]() {
            return ghaFactory->createImage(descriptor).getValue();
        });
    }

    GhaImageView *RgFrameCache::allocateImageView(GhaImage const *const image, GhaImageView::Descriptor descriptor) {
        PoolId viewId{ 0 };
        CacheUtils::hashCombine(viewId, image);
        CacheUtils::hashCombine(viewId, descriptor.type);
        CacheUtils::hashCombine(viewId, descriptor.layer);
        CacheUtils::hashCombine(viewId, descriptor.layerCount);

        return imageViewPool.retrieve(viewId, [&]() {
            return ghaFactory->createImageView(*image, descriptor).getValue();
        });
    }

    GhaFramebuffer *RgFrameCache::allocateFramebuffer(GhaFramebuffer::Descriptor descriptor) {
        PoolId framebufferId{ 0 };
        CacheUtils::hashCombine(framebufferId, descriptor.renderPass);
        for(auto const &imageView : descriptor.attachments) {
            CacheUtils::hashCombine(framebufferId, imageView);
        }
        CacheUtils::hashCombine(framebufferId, descriptor.width);
        CacheUtils::hashCombine(framebufferId, descriptor.height);

        return framebufferPool.retrieve(framebufferId, [&]() {
            return ghaFactory->createFramebuffer(std::move(descriptor)).getValue();
        });
    }

    GhaDescriptorPool *RgFrameCache::allocateDescriptorPool(GhaDescriptorPool::Descriptor descriptor) {
        PoolId poolId{ 0 };
        for(auto &poolType : descriptor.poolTypes) {
            CacheUtils::hashCombine(poolId, poolType.type);
            CacheUtils::hashCombine(poolId, poolType.count);
        }
        CacheUtils::hashCombine(poolId, descriptor.flag);
        CacheUtils::hashCombine(poolId, descriptor.maxSets);

        return descriptorPoolPool.retrieve(poolId, [&]() {
            return ghaFactory->createDescriptorPool(std::move(descriptor)).getValue();
        });
    }

    GhaSemaphore *RgFrameCache::allocateSemaphore() {
        //Always assign a new one until reset.
        static PoolId poolId{ 0 };
        ++poolId;

        return semaphorePool.retrieve(poolId, [&]() {
            return ghaFactory->createSemaphore().getValue();
        });
    }

    GhaGraphicsCommandBuffer *RgFrameCache::allocateGraphicsCommandBuffer() {
        std::unique_ptr<GhaGraphicsCommandBuffer> commandBuffer{ graphicsQueue->allocateCommandBuffer() };
        auto *bufferPtr{ commandBuffer.get() };
        allocatedGraphicsCommandBuffers.push_back(std::move(commandBuffer));
        return bufferPtr;
    }

    GhaComputeCommandBuffer *RgFrameCache::allocateComputeCommandBuffer() {
        std::unique_ptr<GhaComputeCommandBuffer> commandBuffer{ computeQueue->allocateCommandBuffer() };
        auto *bufferPtr{ commandBuffer.get() };
        allocatedComputeCommandBuffers.push_back(std::move(commandBuffer));
        return bufferPtr;
    }

    GhaComputeCommandBuffer *RgFrameCache::allocateAsyncComputeCommandBuffer() {
        std::unique_ptr<GhaComputeCommandBuffer> commandBuffer{ asyncComputeQueue->allocateCommandBuffer() };
        auto *bufferPtr{ commandBuffer.get() };
        allocatedAsyncComputeCommandBuffers.push_back(std::move(commandBuffer));
        return bufferPtr;
    }

    GhaTransferCommandBuffer *RgFrameCache::allocateTransferCommandBuffer() {
        std::unique_ptr<GhaTransferCommandBuffer> commandBuffer{ transferQueue->allocateCommandBuffer() };
        auto *bufferPtr{ commandBuffer.get() };
        allocatedTransferCommandBuffers.push_back(std::move(commandBuffer));
        return bufferPtr;
    }

    void RgFrameCache::submit(GraphicsSubmitInfo submitInfo, GhaFence *signalFence) {//NOLINT false positive - thinks submitInfo is copied
        graphicsQueue->submit({ std::move(submitInfo) }, signalFence);
    }

    void RgFrameCache::submit(ComputeSubmitInfo submitInfo, GhaFence *signalFence, RgSyncType syncType) {//NOLINT false positive - thinks submitInfo is copied
        if(syncType == RgSyncType::Sync) {
            computeQueue->submit({ std::move(submitInfo) }, signalFence);
        } else {
            asyncComputeQueue->submit({ std::move(submitInfo) }, signalFence);
        }
    }

    void RgFrameCache::submit(TransferSubmitInfo submitInfo, GhaFence *signalFence) {//NOLINT false positive - thinks submitInfo is copied
        transferQueue->submit({ std::move(submitInfo) }, signalFence);
    }
}