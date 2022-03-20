#include "Clove/Rendering/Renderables/Mesh.hpp"

#include "Clove/Application.hpp"

#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>

namespace clove {
    namespace {
        void copyFullBuffer(GhaBuffer &source, GhaBuffer &dest, size_t const size) {
            GhaFactory &factory{ *Application::get().getGraphicsDevice()->getGraphicsFactory() };

            auto transferQueue{ factory.createTransferQueue().getValue() };
            auto transferCommandBuffer{ transferQueue->allocateCommandBuffer() };

            transferCommandBuffer->beginRecording();
            transferCommandBuffer->copyBufferToBuffer(source, 0, dest, 0, size);
            transferCommandBuffer->endRecording();

            auto transferQueueFinishedFence{ factory.createFence({ false }).getValue() };

            transferQueue->submit({ TransferSubmitInfo{ .commandBuffers = { transferCommandBuffer.get() } } }, transferQueueFinishedFence.get());

            transferQueueFinishedFence->wait();
            transferQueue->freeCommandBuffer(transferCommandBuffer);
        }
    }

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices)
        : vertices(std::move(vertices))
        , indices(std::move(indices)) {
        GhaFactory &factory{ *Application::get().getGraphicsDevice()->getGraphicsFactory() };

        vertexBufferSize = sizeof(Vertex) * this->vertices.size();
        indexBufferSize  = sizeof(uint16_t) * this->indices.size();
        size_t const totalSize{ vertexBufferSize + indexBufferSize };

        vertexOffset = 0;
        indexOffset  = vertexBufferSize;

        std::unique_ptr<GhaTransferQueue> transferQueue{ factory.createTransferQueue().getValue() };
        std::unique_ptr<GhaTransferCommandBuffer> transferCommandBuffer{ transferQueue->allocateCommandBuffer() };

        //Staging buffer
        auto stagingBuffer{ factory.createBuffer(GhaBuffer::Descriptor{
                                                     .size        = totalSize,
                                                     .usageFlags  = GhaBuffer::UsageMode::TransferSource,
                                                     .sharingMode = SharingMode::Exclusive,
                                                     .memoryType  = MemoryType::SystemMemory,
                                                 })
                                .getValue() };

        //Combined Buffer
        combinedBuffer = factory.createBuffer(GhaBuffer::Descriptor{
            .size        = totalSize,
            .usageFlags  = GhaBuffer::UsageMode::TransferDestination | GhaBuffer::UsageMode::StorageBuffer | GhaBuffer::UsageMode::VertexBuffer | GhaBuffer::UsageMode::IndexBuffer,
            .sharingMode = SharingMode::Concurrent,
            .memoryType  = MemoryType::VideoMemory,
        }).getValue();

        //Map the data into system memory
        stagingBuffer->write(this->vertices.data(), vertexOffset, vertexBufferSize);
        stagingBuffer->write(this->indices.data(), indexOffset, indexBufferSize);

        //Transfer the data to video memory
        transferCommandBuffer->beginRecording();
        transferCommandBuffer->copyBufferToBuffer(*stagingBuffer, 0, *combinedBuffer, 0, totalSize);
        transferCommandBuffer->endRecording();

        auto transferQueueFinishedFence{ factory.createFence({ false }).getValue() };

        transferQueue->submit({ TransferSubmitInfo{ .commandBuffers = { transferCommandBuffer.get() } } }, transferQueueFinishedFence.get());

        transferQueueFinishedFence->wait();
        transferQueue->freeCommandBuffer(transferCommandBuffer);
    }

    Mesh::Mesh(Mesh const &other)
        : vertices{ other.vertices }
        , indices{ other.indices }
        , vertexOffset{ other.vertexOffset }
        , vertexBufferSize{ other.vertexBufferSize }
        , indexOffset{ other.indexOffset }
        , indexBufferSize{ other.indexBufferSize } {

        size_t const totalSize{ vertexBufferSize + indexBufferSize };
        copyFullBuffer(*other.combinedBuffer, *combinedBuffer, totalSize);
    }

    Mesh::Mesh(Mesh &&other) noexcept = default;

    Mesh &Mesh::operator=(Mesh const &other) {
        vertices         = other.vertices;
        indices          = other.indices;
        vertexOffset     = other.vertexOffset;
        vertexBufferSize = other.vertexBufferSize;
        indexOffset      = other.indexOffset;
        indexBufferSize  = other.indexBufferSize;

        size_t const totalSize{ vertexBufferSize + indexBufferSize };
        copyFullBuffer(*other.combinedBuffer, *combinedBuffer, totalSize);

        return *this;
    }

    Mesh &Mesh::operator=(Mesh &&other) noexcept = default;

    Mesh::~Mesh() = default;
}