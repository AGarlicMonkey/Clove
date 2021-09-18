#pragma once

#include "Clove/Rendering/RenderTarget.hpp"

#include <Clove/Delegate/DelegateHandle.hpp>
#include <vector>

namespace clove {
    class GhaSwapchain;
    class GhaPresentQueue;
    class GhaGraphicsQueue;
    class GhaDevice;
    class GhaFactory;
    class GhaFence;
    class Window;
}

namespace clove {
    /**
     * @brief GhaSwapchain backed RenderTarget.
     */
    class SwapchainRenderTarget : public RenderTarget {
        //VARIABLES
    private:
        GhaDevice *graphicsDevice{ nullptr };
        GhaFactory *graphicsFactory{ nullptr };

        std::unique_ptr<GhaSwapchain> swapchain;
        std::unique_ptr<GhaPresentQueue> presentQueue;
        std::unique_ptr<GhaGraphicsQueue> graphicsQueue;

        std::vector<std::unique_ptr<GhaSemaphore>> renderFinishedSemaphores;
        std::vector<std::unique_ptr<GhaSemaphore>> imageAvailableSemaphores;
        std::vector<std::unique_ptr<GhaFence>> framesInFlight;
        std::vector<GhaFence *> imagesInFlight;

        vec2ui windowSize{};
        DelegateHandle windowResizeHandle;

        bool requiresNewSwapchain{ false };

        //FUNCTIONS
    public:
        SwapchainRenderTarget() = delete;
        SwapchainRenderTarget(Window &swapchainWindow, GhaDevice *graphicsDevice);

        SwapchainRenderTarget(SwapchainRenderTarget const &other);
        SwapchainRenderTarget(SwapchainRenderTarget &&other) noexcept;

        SwapchainRenderTarget &operator=(SwapchainRenderTarget const &other);
        SwapchainRenderTarget &operator=(SwapchainRenderTarget &&other) noexcept;

        ~SwapchainRenderTarget();

        Expected<uint32_t, std::string> aquireNextImage(size_t const frameId) override;

        void submit(uint32_t imageIndex, size_t const frameId, GraphicsSubmitInfo submission) override;

        GhaImage::Format getImageFormat() const override;
        vec2ui getSize() const override;

        std::vector<GhaImageView *> getImageViews() const override;

    private:
        void onSurfaceSizeChanged(vec2ui const &size);
        void createSwapchain();
    };
}