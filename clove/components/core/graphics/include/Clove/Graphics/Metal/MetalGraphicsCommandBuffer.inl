namespace clove {
    id<MTLCommandBuffer> MetalGraphicsCommandBuffer::getMtlCommandBuffer() const {
        return commandBuffer;
    }

    std::vector<id<MTLRenderCommandEncoder>> const &MetalGraphicsCommandBuffer::getRenderPasses() const {
        return passes;
    }
}
