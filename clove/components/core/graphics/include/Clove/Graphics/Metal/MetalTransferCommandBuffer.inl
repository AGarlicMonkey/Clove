namespace clove {
    id<MTLCommandBuffer> MetalTransferCommandBuffer::getMtlCommandBuffer() const {
        return commandBuffer;
    }

    id<MTLBlitCommandEncoder> MetalTransferCommandBuffer::getEncoder() const {
        return encoder;
    }
}
