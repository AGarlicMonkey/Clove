#pragma once

#include "Clove/Rendering/RenderGraph/RgId.hpp"
#include "Clove/Rendering/RenderGraph/RgTypes.hpp"

#include <Clove/Graphics/GhaBuffer.hpp>
#include <Clove/Graphics/GhaComputeCommandBuffer.hpp>
#include <Clove/Graphics/GhaDescriptorPool.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaFramebuffer.hpp>
#include <Clove/Graphics/GhaGraphicsCommandBuffer.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Graphics/GhaImageView.hpp>
#include <Clove/Graphics/GhaTransferCommandBuffer.hpp>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace clove {
    class GhaSemaphore;
}

namespace clove {
    /**
     * @brief Stores objects associated with a single frame (images, buffers, etc.) for a RenderGraph
     */
    class RgFrameCache {
        //TYPES
    private:
        using PoolId = uint64_t;

        template<typename T>
        struct ResourcePool {
            std::unordered_map<PoolId, std::vector<std::unique_ptr<T>>> free{};
            std::unordered_map<PoolId, std::vector<std::unique_ptr<T>>> allocated{};

            /**
             * @brief Retrieve an element from the pool. Allocating a new one if necessary.
             * @param id Unique id of an element to retrieve.
             * @param createItem Functor that will be called if a new item needs to be created.
             * @return 
             */
            T *retrieve(PoolId const id, std::function<std::unique_ptr<T>()> createItem) {
                std::unique_ptr<T> item{};
                bool const containsFreeElement{ free.contains(id) && !free.at(id).empty() };
                if(containsFreeElement) {
                    auto &freeVec{ free.at(id) };

                    item = std::move(freeVec.back());
                    freeVec.pop_back();
                } else {
                    item = createItem();
                }
                allocated[id].push_back(std::move(item));

                return allocated[id].back().get();
            }

            /**
             * @brief Resets the pool marking all allocated elements as free.
             */
            void reset() {
                for(auto &&[key, vec] : allocated) {
                    for(auto &item : vec) {
                        free[key].push_back(std::move(item));
                    }
                }
                allocated.clear();
            }

            void clear() {
                free.clear();
                allocated.clear();
            }
        };

        //VARIABLES
    private:
        GhaFactory *ghaFactory{ nullptr };

        ResourcePool<GhaBuffer> bufferPool{};
        ResourcePool<GhaImage> imagePool{};
        ResourcePool<GhaImageView> imageViewPool{};
        ResourcePool<GhaFramebuffer> framebufferPool{};
        ResourcePool<GhaDescriptorPool> descriptorPoolPool{};
        ResourcePool<GhaSemaphore> semaphorePool{};

        GhaGraphicsQueue *graphicsQueue{ nullptr };
        GhaComputeQueue *computeQueue{ nullptr };
        GhaComputeQueue *asyncComputeQueue{ nullptr };
        GhaTransferQueue *transferQueue{ nullptr };

        std::vector<std::unique_ptr<GhaGraphicsCommandBuffer>> allocatedGraphicsCommandBuffers{};
        std::vector<std::unique_ptr<GhaComputeCommandBuffer>> allocatedComputeCommandBuffers{};
        std::vector<std::unique_ptr<GhaComputeCommandBuffer>> allocatedAsyncComputeCommandBuffers{};
        std::vector<std::unique_ptr<GhaTransferCommandBuffer>> allocatedTransferCommandBuffers{};

        //FUNCTIONS
    public:
        RgFrameCache() = delete;
        RgFrameCache(GhaFactory *ghaFactory, GhaGraphicsQueue *graphicsQueue, GhaComputeQueue *computeQueue, GhaComputeQueue *asyncComputeQueue, GhaTransferQueue *transferQueue);

        RgFrameCache(RgFrameCache const &other) = delete;
        RgFrameCache(RgFrameCache &&other) noexcept;

        RgFrameCache &operator=(RgFrameCache const &other) = delete;
        RgFrameCache &operator=(RgFrameCache &&other) noexcept;

        ~RgFrameCache();

        /**
         * @brief Marks all allocated objects as free to use.
         */
        void reset();
        /**
         * @brief Deletes all stored objects in the cache.
         */
        void clear();

        GhaBuffer *allocateBuffer(GhaBuffer::Descriptor descriptor);

        GhaImage *allocateImage(GhaImage::Descriptor descriptor);
        GhaImageView *allocateImageView(GhaImage const *const image, GhaImageView::Descriptor descriptor);

        GhaFramebuffer *allocateFramebuffer(GhaFramebuffer::Descriptor descriptor);

        GhaDescriptorPool *allocateDescriptorPool(GhaDescriptorPool::Descriptor descriptor);

        GhaSemaphore *allocateSemaphore();

        GhaGraphicsCommandBuffer *allocateGraphicsCommandBuffer();
        GhaComputeCommandBuffer *allocateComputeCommandBuffer();
        GhaComputeCommandBuffer *allocateAsyncComputeCommandBuffer();
        GhaTransferCommandBuffer *allocateTransferCommandBuffer();

        void submit(GraphicsSubmitInfo submitInfo, GhaFence *signalFence);
        void submit(ComputeSubmitInfo submitInfo, GhaFence *signalFence, RgSyncType syncType);
        void submit(TransferSubmitInfo submitInfo, GhaFence *signalFence);
    };
}