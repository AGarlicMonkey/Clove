#pragma once

#include "Clove/Graphics/GhaSwapchain.hpp"

#include <queue>
#include <MetalKit/MetalKit.h>
#include <mutex>

@class MetalView;

namespace clove {
    class MetalImage;
}

namespace clove {
    class MetalSwapchain : public GhaSwapchain {
        //VARIABLES
    private:
        MetalView *view{ nullptr };
        id<MTLCommandQueue> presentQueue{};
        
        uint32_t activeImage{ 0 };
        
        GhaImage::Format imageFormat{};
        vec2ui imageSize{};
        
        std::vector<std::unique_ptr<MetalImage>> images{};
        
        std::queue<uint32_t> imageQueue{};
        std::mutex imageQueueMutex{};
        
        //FUNCTIONS
    public:
        MetalSwapchain() = delete;
        MetalSwapchain(MetalView *view, id<MTLCommandQueue> presentQueue, std::vector<std::unique_ptr<MetalImage>> images, GhaImage::Format imageFormat, vec2ui imageSize);
        
        MetalSwapchain(MetalSwapchain const &other) = delete;
        MetalSwapchain(MetalSwapchain &&other) noexcept = delete;
        
        MetalSwapchain& operator=(MetalSwapchain const &other) = delete;
        MetalSwapchain& operator=(MetalSwapchain &&other) noexcept = delete;
        
        ~MetalSwapchain();
        
        std::pair<GhaImage *, Result> aquireNextImage(GhaSemaphore const *availableSemaphore) override;
        
        uint32_t getCurrentImageIndex() const override;
        
        Result present(std::vector<GhaSemaphore const *> waitSemaphores) override;
        
        GhaImage::Format getImageFormat() const override;
        vec2ui getSize() const override;
    };
}
