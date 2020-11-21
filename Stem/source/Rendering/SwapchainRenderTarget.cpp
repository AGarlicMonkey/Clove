#include "Stem/Rendering/SwapchainRenderTarget.hpp"

#include "Stem/Application.hpp"

#include <Clove/Graphics/Fence.hpp>
#include <Clove/Graphics/GraphicsDevice.hpp>
#include <Clove/Graphics/GraphicsFactory.hpp>
#include <Clove/Graphics/PresentQueue.hpp>
#include <Clove/Graphics/Semaphore.hpp>
#include <Clove/Graphics/Swapchain.hpp>
#include <Clove/Platform/Window.hpp>
#include <Root/Log/Log.hpp>

namespace garlic::inline stem {
    SwapchainRenderTarget::SwapchainRenderTarget()
        : graphicsDevice{ Application::get().getGraphicsDevice() } {
        graphicsFactory = graphicsDevice->getGraphicsFactory();

        auto const window{ Application::get().getWindow() };

        windowSize         = window->getSize();
        windowResizeHandle = window->onWindowResize.bind(&SwapchainRenderTarget::onWindowSizeChanged, this);

        auto expectedPresentQueue{ graphicsFactory->createPresentQueue() };
        if(expectedPresentQueue.hasValue()) {
            presentQueue = std::move(expectedPresentQueue.getValue());
        } else {
            GARLIC_ASSERT(false, expectedPresentQueue.getError());
        }

        //We won't be allocating any buffers from this queue, only using it to submit
        graphicsQueue = graphicsFactory->createGraphicsQueue(clv::gfx::CommandQueueDescriptor{ .flags = clv::gfx::QueueFlags::None });

        createSwapchain();
    }

    SwapchainRenderTarget::SwapchainRenderTarget(SwapchainRenderTarget &&other) noexcept = default;

    SwapchainRenderTarget &SwapchainRenderTarget::operator=(SwapchainRenderTarget &&other) noexcept = default;

    SwapchainRenderTarget::~SwapchainRenderTarget() = default;

    Expected<uint32_t, std::string> SwapchainRenderTarget::aquireNextImage(size_t const frameId) {
        if(windowSize.x == 0 || windowSize.y == 0) {
            return Unexpected<std::string>{ "Cannot acquire image while Window is minimised." };
        }

        if(std::size(imageAvailableSemaphores) <= frameId) {
            imageAvailableSemaphores.emplace_back(graphicsFactory->createSemaphore());
        }
        if(std::size(framesInFlight) <= frameId) {
            framesInFlight.emplace_back(graphicsFactory->createFence({ true }));
        }

        //Wait for the graphics queue to be finished with the frame we want to render
        framesInFlight[frameId]->wait();

        auto const [imageIndex, result] = swapchain->aquireNextImage(imageAvailableSemaphores[frameId].get());
        if(result == clv::gfx::Result::Error_SwapchainOutOfDate) {
            createSwapchain();
            return Unexpected<std::string>{ "Swapchain was recreated." };
        }

        //Make sure we're not about to start using an image that hasn't been rendered to yet
        if(imagesInFlight[imageIndex] != nullptr) {
            imagesInFlight[imageIndex]->wait();
        }
        imagesInFlight[imageIndex] = framesInFlight[frameId];

        framesInFlight[frameId]->reset();

        return imageIndex;
    }

    void SwapchainRenderTarget::submit(uint32_t imageIndex, size_t const frameId, clv::gfx::GraphicsSubmitInfo primarySubmission, std::vector<clv::gfx::GraphicsSubmitInfo> secondarySubmissions) {
        if(std::size(renderFinishedSemaphores) <= frameId) {
            renderFinishedSemaphores.emplace_back(graphicsFactory->createSemaphore());
        }

        //Inject the sempahores we use to synchronise with the swapchain and present queue
        primarySubmission.waitSemaphores.push_back({ imageAvailableSemaphores[frameId], clv::gfx::PipelineObject::Stage::ColourAttachmentOutput });
        primarySubmission.signalSemaphores.push_back(renderFinishedSemaphores[frameId]);

        secondarySubmissions.emplace_back(std::move(primarySubmission));
        graphicsQueue->submit(secondarySubmissions, framesInFlight[frameId].get());

        auto const result = presentQueue->present(clv::gfx::PresentInfo{
            .waitSemaphores = { renderFinishedSemaphores[frameId] },
            .swapChain      = swapchain,
            .imageIndex     = imageIndex,
        });

        if(result == clv::gfx::Result::Error_SwapchainOutOfDate || result == clv::gfx::Result::Success_SwapchainSuboptimal) {
            createSwapchain();
        }
    }

    clv::gfx::GraphicsImage::Format SwapchainRenderTarget::getImageFormat() const {
        return swapchain->getImageFormat();
    }

    clv::mth::vec2ui SwapchainRenderTarget::getSize() const {
        return swapchain->getSize();
    }

    std::vector<std::shared_ptr<clv::gfx::GraphicsImageView>> SwapchainRenderTarget::getImageViews() const {
        return swapchain->getImageViews();
    }

    void SwapchainRenderTarget::onWindowSizeChanged(clv::mth::vec2ui const &size) {
        windowSize = size;
        createSwapchain();
    }

    void SwapchainRenderTarget::createSwapchain() {
        if(windowSize.x == 0 || windowSize.y == 0) {
            return;
        }

        graphicsDevice->waitForIdleDevice();

        auto expectedSwapchain{ graphicsFactory->createSwapChain({ windowSize }) };
        if(expectedSwapchain.hasValue()) {
            swapchain = std::move(expectedSwapchain.getValue());
        } else {
            GARLIC_ASSERT(false, expectedSwapchain.getError());
        }

        imagesInFlight.resize(std::size(swapchain->getImageViews()));

        onPropertiesChanged.broadcast();
    }
}