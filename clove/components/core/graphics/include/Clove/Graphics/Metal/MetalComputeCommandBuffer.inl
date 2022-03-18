namespace clove {
    id<MTLCommandBuffer> MetalComputeCommandBuffer::getMtlCommandBuffer() const {
        return commandBuffer;
    }

    id<MTLComputeCommandEncoder> MetalComputeCommandBuffer::getEncoder() const {
        return encoder;
    }
}
