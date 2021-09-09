#include "Clove/Rendering/SwapchainRenderTarget.hpp"

#include "Clove/Application.hpp"
#include "Clove/Surface.hpp"

#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaFence.hpp>
#include <Clove/Graphics/GhaPresentQueue.hpp>
#include <Clove/Graphics/GhaSemaphore.hpp>
#include <Clove/Graphics/GhaSwapchain.hpp>
#include <Clove/Log/Log.hpp>

namespace clove {
    SwapchainRenderTarget::SwapchainRenderTarget(Surface &swapchainSurface, GhaDevice *graphicsDevice)
        : graphicsDevice{ graphicsDevice } {
        graphicsFactory = graphicsDevice->getGraphicsFactory();

        surfaceSize         = swapchainSurface.getSize();
        surfaceResizeHandle = swapchainSurface.onSurfaceResize().bind(&SwapchainRenderTarget::onSurfaceSizeChanged, this);

        presentQueue = graphicsFactory->createPresentQueue().getValue();

        //We won't be allocating any buffers from this queue, only using it to submit
        graphicsQueue = graphicsFactory->createGraphicsQueue(CommandQueueDescriptor{ .flags = QueueFlags::None }).getValue();

        createSwapchain();
    }

    SwapchainRenderTarget::SwapchainRenderTarget(SwapchainRenderTarget &&other) noexcept = default;

    SwapchainRenderTarget &SwapchainRenderTarget::operator=(SwapchainRenderTarget &&other) noexcept = default;

    SwapchainRenderTarget::~SwapchainRenderTarget() = default;

    Expected<uint32_t, std::string> SwapchainRenderTarget::aquireNextImage(size_t const frameId) {
        if(surfaceSize.x == 0 || surfaceSize.y == 0) {
            return Unexpected<std::string>{ "Cannot acquire image while Window is minimised." };
        }

        if(requiresNewSwapchain) {
            createSwapchain();
            return Unexpected<std::string>{ "GhaSwapchain was recreated." };
        }

        if(std::size(imageAvailableSemaphores) <= frameId) {
            imageAvailableSemaphores.emplace_back(graphicsFactory->createSemaphore().getValue());
        }
        if(std::size(framesInFlight) <= frameId) {
            framesInFlight.emplace_back(graphicsFactory->createFence({ true }).getValue());
        }

        //Wait for the graphics queue to be finished with the frame we want to render
        framesInFlight[frameId]->wait();

        auto const [imageIndex, result] = swapchain->aquireNextImage(imageAvailableSemaphores[frameId].get());
        if(result == Result::Error_SwapchainOutOfDate) {
            createSwapchain();
            return Unexpected<std::string>{ "GhaSwapchain was recreated." };
        }

        //Make sure we're not about to start using an image that hasn't been rendered to yet
        if(imagesInFlight[imageIndex] != nullptr) {
            imagesInFlight[imageIndex]->wait();
        }
        imagesInFlight[imageIndex] = framesInFlight[frameId].get();

        framesInFlight[frameId]->reset();

        return imageIndex;
    }

    void SwapchainRenderTarget::submit(uint32_t imageIndex, size_t const frameId, GraphicsSubmitInfo submission) {
        if(std::size(renderFinishedSemaphores) <= frameId) {
            renderFinishedSemaphores.emplace_back(graphicsFactory->createSemaphore().getValue());
        }

        //Inject the sempahores we use to synchronise with the swapchain and present queue
        submission.waitSemaphores.emplace_back(imageAvailableSemaphores[frameId].get(), PipelineStage::ColourAttachmentOutput);
        submission.signalSemaphores.push_back(renderFinishedSemaphores[frameId].get());

        graphicsQueue->submit({ std::move(submission) }, framesInFlight[frameId].get());

        auto const result = presentQueue->present(PresentInfo{
            .waitSemaphores = { renderFinishedSemaphores[frameId].get() },
            .swapChain      = swapchain.get(),
            .imageIndex     = imageIndex,
        });

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

    std::vector<GhaImageView *> SwapchainRenderTarget::getImageViews() const {
        return swapchain->getImageViews();
    }

    void SwapchainRenderTarget::onSurfaceSizeChanged(vec2ui const &size) {
        surfaceSize          = size;
        requiresNewSwapchain = true;
    }

    void SwapchainRenderTarget::createSwapchain() {
        requiresNewSwapchain = true;

        if(surfaceSize.x == 0 || surfaceSize.y == 0) {
            return;
        }

        onPropertiesChangedBegin.broadcast();

        graphicsDevice->waitForIdleDevice();

        swapchain.reset();
        swapchain = graphicsFactory->createSwapChain({ surfaceSize }).getValue();

        imagesInFlight.resize(std::size(swapchain->getImageViews()));

        onPropertiesChangedEnd.broadcast();

        requiresNewSwapchain = false;
    }
}