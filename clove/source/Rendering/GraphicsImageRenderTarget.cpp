#include "Clove/Rendering/GraphicsImageRenderTarget.hpp"

#include "Clove/Application.hpp"

#include <Clove/Graphics/GhaBuffer.hpp>
#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaFence.hpp>
#include <Clove/Graphics/GhaGraphicsQueue.hpp>

namespace garlic::clove {
    GraphicsImageRenderTarget::GraphicsImageRenderTarget(GhaImage::Descriptor imageDescriptor, std::shared_ptr<GhaFactory> factory)
        : imageDescriptor{ imageDescriptor }
        , factory{ std::move(factory) } {
        transferQueue         = *this->factory->createTransferQueue(CommandQueueDescriptor{ .flags = QueueFlags::ReuseBuffers });
        transferCommandBuffer = transferQueue->allocateCommandBuffer();

        frameInFlight = *this->factory->createFence({ true });

        createImages();
    }

    GraphicsImageRenderTarget::GraphicsImageRenderTarget(GraphicsImageRenderTarget &&other) noexcept = default;

    GraphicsImageRenderTarget &GraphicsImageRenderTarget::operator=(GraphicsImageRenderTarget &&other) noexcept = default;

    GraphicsImageRenderTarget::~GraphicsImageRenderTarget() = default;

    Expected<uint32_t, std::string> GraphicsImageRenderTarget::aquireNextImage(std::shared_ptr<GhaSemaphore> signalSemaphore) {
        //A bit hacky but inject an empty submission to signal the provided semaphore.
        //This will execute after what ever is in the current queue so will technicaly signal once the transfer queue
        //is ready to transfer another image
        if(signalSemaphore != nullptr) {
            TransferSubmitInfo transferSubmission{
                .signalSemaphores = { std::move(signalSemaphore) },
            };
            transferQueue->submit({ std::move(transferSubmission) }, nullptr);
        }

        return 0; //Only a single backing image for now
    }

    void GraphicsImageRenderTarget::present(uint32_t imageIndex, std::vector<std::shared_ptr<GhaSemaphore>> waitSemaphores) {
        //Fences can't be resubmitted in a signaled state. So we have to wait for and then reset the fence.
        frameInFlight->wait();
        frameInFlight->reset();

        std::vector<std::pair<std::shared_ptr<GhaSemaphore>, PipelineStage>> transferWaitSemaphores;
        for(auto const &semaphore : waitSemaphores) {
            transferWaitSemaphores.push_back(std::make_pair(semaphore, PipelineStage::Transfer));
        }

        TransferSubmitInfo transferSubmission{
            .waitSemaphores = transferWaitSemaphores,
            .commandBuffers = { transferCommandBuffer },
        };
        transferQueue->submit({ std::move(transferSubmission) }, frameInFlight.get());
    }

    GhaImage::Format GraphicsImageRenderTarget::getImageFormat() const {
        return imageDescriptor.format;
    }

    vec2ui GraphicsImageRenderTarget::getSize() const {
        return imageDescriptor.dimensions;
    }

    std::vector<std::shared_ptr<GhaImageView>> GraphicsImageRenderTarget::getImageViews() const {
        return { renderTargetView };
    }

    void GraphicsImageRenderTarget::resize(vec2ui size) {
        imageDescriptor.dimensions = size;
        createImages();
    }

    std::shared_ptr<GhaBuffer> GraphicsImageRenderTarget::getNextReadyBuffer() {
        //Stall until we are ready to return the image.
        frameInFlight->wait();

        return renderTargetBuffer;
    }

    void GraphicsImageRenderTarget::createImages() {
        onPropertiesChangedBegin.broadcast();

        renderTargetImage = *factory->createImage(imageDescriptor);
        renderTargetView  = *factory->createImageView(*renderTargetImage, GhaImageView::Descriptor{
                                                                             .type       = GhaImageView::Type::_2D,
                                                                             .layer      = 0,
                                                                             .layerCount = 1,
                                                                         });

        size_t constexpr bytesPerPixel{ 4 };//Assuming image format is 4 bbp
        size_t const bufferSize{ imageDescriptor.dimensions.x * imageDescriptor.dimensions.y * bytesPerPixel };
        renderTargetBuffer = *factory->createBuffer(GhaBuffer::Descriptor{
            .size        = bufferSize,
            .usageFlags  = GhaBuffer::UsageMode::TransferDestination,
            .sharingMode = SharingMode::Exclusive,
            .memoryType  = MemoryType::SystemMemory,
        });

        //Pre-record the transfer command
        ImageMemoryBarrierInfo constexpr layoutTransferInfo{
            .currentAccess      = AccessFlags::None,
            .newAccess          = AccessFlags::TransferRead,
            .currentImageLayout = GhaImage::Layout::Undefined,
            .newImageLayout     = GhaImage::Layout::TransferSourceOptimal,
            .sourceQueue        = QueueType::None,
            .destinationQueue   = QueueType::None,
        };

        transferCommandBuffer->beginRecording(CommandBufferUsage::Default);
        transferCommandBuffer->imageMemoryBarrier(*renderTargetImage, layoutTransferInfo, PipelineStage::Top, PipelineStage::Transfer);
        transferCommandBuffer->copyImageToBuffer(*renderTargetImage, { 0, 0, 0 }, { imageDescriptor.dimensions, 1 }, *renderTargetBuffer, 0);
        transferCommandBuffer->endRecording();

        onPropertiesChangedEnd.broadcast();
    }
}