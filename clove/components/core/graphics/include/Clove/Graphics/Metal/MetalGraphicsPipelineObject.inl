namespace clove {
    id<MTLRenderPipelineState> MetalGraphicsPipelineObject::getPipeline() const {
        return pipeline;
    }

    id<MTLDepthStencilState> MetalGraphicsPipelineObject::getDepthStencil() const {
        return depthStencil;
    }
}
