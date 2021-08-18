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
    class Surface;
}

namespace clove {
    /**
     * @brief GhaSwapchain backed RenderTarget.
     */
    class SwapchainRenderTarget : public RenderTarget {
        //VARIABLES
    private:
        GhaDevice *graphicsDevice{ nullptr };
        std::shared_ptr<GhaFactory> graphicsFactory;

        std::shared_ptr<GhaSwapchain> swapchain;
        std::shared_ptr<GhaPresentQueue> presentQueue;
        std::shared_ptr<GhaGraphicsQueue> graphicsQueue;

        std::vector<std::shared_ptr<GhaSemaphore>> renderFinishedSemaphores;
        std::vector<std::shared_ptr<GhaSemaphore>> imageAvailableSemaphores;
        std::vector<std::shared_ptr<GhaFence>> framesInFlight;
        std::vector<std::shared_ptr<GhaFence>> imagesInFlight;

        vec2ui surfaceSize{};
        DelegateHandle surfaceResizeHandle;

        bool requiresNewSwapchain{ false };

        //FUNCTIONS
    public:
        SwapchainRenderTarget() = delete;
        SwapchainRenderTarget(Surface &swapchainSurface, GhaDevice *graphicsDevice);

        SwapchainRenderTarget(SwapchainRenderTarget const &other);
        SwapchainRenderTarget(SwapchainRenderTarget &&other) noexcept;

        SwapchainRenderTarget &operator=(SwapchainRenderTarget const &other);
        SwapchainRenderTarget &operator=(SwapchainRenderTarget &&other) noexcept;

        ~SwapchainRenderTarget();

        Expected<uint32_t, std::string> aquireNextImage(size_t const frameId) override;

        void submit(uint32_t imageIndex, size_t const frameId, GraphicsSubmitInfo submission) override;

        GhaImage::Format getImageFormat() const override;
        vec2ui getSize() const override;

        std::vector<std::shared_ptr<GhaImageView>> getImageViews() const override;

    private:
        void onSurfaceSizeChanged(vec2ui const &size);
        void createSwapchain();
    };
}