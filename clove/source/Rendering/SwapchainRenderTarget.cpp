#include "Clove/Rendering/SwapchainRenderTarget.hpp"

#include "Clove/Application.hpp"

#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaFence.hpp>
#include <Clove/Graphics/GhaSemaphore.hpp>
#include <Clove/Graphics/GhaSwapchain.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Platform/Window.hpp>

namespace clove {
    SwapchainRenderTarget::SwapchainRenderTarget(Window &swapchainWindow, GhaDevice *graphicsDevice, uint32_t imageCount)
        : graphicsDevice{ graphicsDevice }
        , imageCount{ imageCount } {
        graphicsFactory = graphicsDevice->getGraphicsFactory();

        windowSize         = swapchainWindow.getSize(true);
        windowResizeHandle = swapchainWindow.onWindowResize.bind(&SwapchainRenderTarget::onSurfaceSizeChanged, this);

        createSwapchain();
    }

    SwapchainRenderTarget::SwapchainRenderTarget(SwapchainRenderTarget &&other) noexcept = default;

    SwapchainRenderTarget &SwapchainRenderTarget::operator=(SwapchainRenderTarget &&other) noexcept = default;

    SwapchainRenderTarget::~SwapchainRenderTarget() = default;

    Expected<GhaImage *, std::string> SwapchainRenderTarget::aquireNextImage(GhaSemaphore const *const signalSemaphore) {
        if(windowSize.x == 0 || windowSize.y == 0) {
            return Unexpected<std::string>{ "Cannot acquire image while Window is minimised." };
        }

        if(requiresNewSwapchain) {
            createSwapchain();
            return Unexpected<std::string>{ "GhaSwapchain was recreated." };
        }

        auto const [image, result] = swapchain->aquireNextImage(signalSemaphore);
        if(result == Result::Error_SwapchainOutOfDate) {
            createSwapchain();
            return Unexpected<std::string>{ "GhaSwapchain was recreated." };
        }

        return image;
    }

    uint32_t SwapchainRenderTarget::getCurrentImageIndex() const {
        return swapchain->getCurrentImageIndex();
    }

    void SwapchainRenderTarget::present(std::vector<GhaSemaphore const *> waitSemaphores) {
        Result const result{ swapchain->present(std::move(waitSemaphores)) };

        if(result == Result::Error_SwapchainOutOfDate || result == Result::Success_SwapchainSuboptimal) {
            createSwapchain();
        }
    }

    GhaImage::Format SwapchainRenderTarget::getImageFormat() const {
        return swapchain->getImageFormat();
    }

    vec2ui SwapchainRenderTarget::getSize() const {
        return swapchain->getSize();
    }

    uint32_t SwapchainRenderTarget::getImageCount() const {
        return imageCount;
    }

    void SwapchainRenderTarget::onSurfaceSizeChanged(vec2ui const &size) {
        windowSize           = size;
        requiresNewSwapchain = true;
    }

    void SwapchainRenderTarget::createSwapchain() {
        requiresNewSwapchain = true;

        if(windowSize.x == 0 || windowSize.y == 0) {
            return;
        }

        onPropertiesChangedBegin.broadcast();

        graphicsDevice->waitForIdleDevice();

        swapchain.reset();
        swapchain = graphicsFactory->createSwapChain(GhaSwapchain::Descriptor{
                                                         .extent     = windowSize,
                                                         .imageCount = imageCount,
                                                     })
                        .getValue();

        onPropertiesChangedEnd.broadcast();

        requiresNewSwapchain = false;
    }
}