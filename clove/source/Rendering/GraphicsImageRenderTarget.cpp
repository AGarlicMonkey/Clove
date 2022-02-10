#include "Clove/Rendering/GraphicsImageRenderTarget.hpp"

#include "Clove/Application.hpp"

#include <Clove/Graphics/GhaBuffer.hpp>
#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaFence.hpp>
#include <Clove/Graphics/GhaGraphicsQueue.hpp>

namespace clove {
    GraphicsImageRenderTarget::GraphicsImageRenderTarget(GhaImage::Descriptor imageDescriptor, GhaFactory *factory)
        : imageDescriptor{ imageDescriptor }
        , factory{ factory } {
        transferQueue         = this->factory->createTransferQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers }).getValue();
        transferCommandBuffer = transferQueue->allocateCommandBuffer();

        frameInFlight = this->factory->createFence({ true }).getValue();

        createImages();
    }

    GraphicsImageRenderTarget::GraphicsImageRenderTarget(GraphicsImageRenderTarget &&other) noexcept = default;

    GraphicsImageRenderTarget &GraphicsImageRenderTarget::operator=(GraphicsImageRenderTarget &&other) noexcept = default;

    GraphicsImageRenderTarget::~GraphicsImageRenderTarget() = default;

    Expected<uint32_t, std::string> GraphicsImageRenderTarget::aquireNextImage(GhaSemaphore const *const signalSemaphore) {
        if(requiresResize) {
            createImages();
            return Unexpected<std::string>{ "Backbuffer was recreated." };
        }

        //A bit hacky but inject an empty submission to signal the provided semaphore.
        //This will execute after what ever is in the current queue so will technicaly signal once the transfer queue
        //is ready to transfer another image
        if(signalSemaphore != nullptr) {
            TransferSubmitInfo transferSubmission{
                .signalSemaphores = { signalSemaphore },
            };
            transferQueue->submit({ std::move(transferSubmission) }, nullptr);
        }

        return 0; //Only a single backing image for now
    }

    void GraphicsImageRenderTarget::present(uint32_t imageIndex, std::vector<GhaSemaphore const *> waitSemaphores) {
        //Fences can't be resubmitted in a signaled state. So we have to wait for and then reset the fence.
        frameInFlight->wait();
        frameInFlight->reset();

        std::vector<std::pair<GhaSemaphore const *, PipelineStage>> transferWaitSemaphores;
        transferWaitSemaphores.reserve(waitSemaphores.size());
        for(auto const &semaphore : waitSemaphores) {
            transferWaitSemaphores.emplace_back(semaphore, PipelineStage::Transfer);
        }

        TransferSubmitInfo transferSubmission{
            .waitSemaphores = transferWaitSemaphores,
            .commandBuffers = { transferCommandBuffer.get() },
        };
        transferQueue->submit({ std::move(transferSubmission) }, frameInFlight.get());
    }

    GhaImage::Format GraphicsImageRenderTarget::getImageFormat() const {
        return imageDescriptor.format;
    }

    vec2ui GraphicsImageRenderTarget::getSize() const {
        return imageDescriptor.dimensions;
    }

    std::vector<GhaImage *> GraphicsImageRenderTarget::getImages() const {
        return { renderTargetImage.get() };
    }

    void GraphicsImageRenderTarget::resize(vec2ui size) {
        imageDescriptor.dimensions = size;
        requiresResize             = true;
    }

    GhaBuffer *GraphicsImageRenderTarget::getNextReadyBuffer() {
        //Stall until we are ready to return the image.
        frameInFlight->wait();

        return renderTargetBuffer.get();
    }

    void GraphicsImageRenderTarget::createImages() {
        //Make sure we're not using the image when we re-create it
        frameInFlight->wait();

        onPropertiesChangedBegin.broadcast();

        renderTargetImage = factory->createImage(imageDescriptor).getValue();

        size_t constexpr bytesPerPixel{ 4 };//Assuming image format is 4 bbp
        size_t const bufferSize{ imageDescriptor.dimensions.x * imageDescriptor.dimensions.y * bytesPerPixel };
        renderTargetBuffer = factory->createBuffer(GhaBuffer::Descriptor{
                                                       .size        = bufferSize,
                                                       .usageFlags  = GhaBuffer::UsageMode::TransferDestination,
                                                       .sharingMode = SharingMode::Exclusive,
                                                       .memoryType  = MemoryType::SystemMemory,
                                                   })
                                 .getValue();

        //Pre-record the transfer command
        ImageMemoryBarrierInfo constexpr layoutTransferInfo{
            .currentAccess      = AccessFlags::None,
            .newAccess          = AccessFlags::TransferRead,
            .currentImageLayout = GhaImage::Layout::Present,
            .newImageLayout     = GhaImage::Layout::TransferSourceOptimal,
            .sourceQueue        = QueueType::None,
            .destinationQueue   = QueueType::None,
        };

        transferCommandBuffer->beginRecording(CommandBufferUsage::Default);
        transferCommandBuffer->imageMemoryBarrier(*renderTargetImage, layoutTransferInfo, PipelineStage::Top, PipelineStage::Transfer);
        transferCommandBuffer->copyImageToBuffer(*renderTargetImage, { 0, 0, 0 }, { imageDescriptor.dimensions, 1 }, 0, 1, *renderTargetBuffer, 0);
        transferCommandBuffer->endRecording();

        onPropertiesChangedEnd.broadcast();

        requiresResize = false;
    }
}