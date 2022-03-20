
namespace clove {
    //Graphics
    template<typename BaseCommandBufferType>
    void ValidationGraphicsCommandBuffer<BaseCommandBufferType>::beginRecording() {
        validateBeginRecording();
        BaseCommandBufferType::beginRecording();
    }

    template<typename BaseCommandBufferType>
    void ValidationGraphicsCommandBuffer<BaseCommandBufferType>::endRecording() {
        onEndRecording();
        BaseCommandBufferType::endRecording();
    }

    //Compute
    template<typename BaseCommandBufferType>
    void ValidationComputeCommandBuffer<BaseCommandBufferType>::beginRecording() {
        validateBeginRecording();
        BaseCommandBufferType::beginRecording();
    }

    template<typename BaseCommandBufferType>
    void ValidationComputeCommandBuffer<BaseCommandBufferType>::endRecording() {
        onEndRecording();
        BaseCommandBufferType::endRecording();
    }

    //Transfer
    template<typename BaseCommandBufferType>
    void ValidationTransferCommandBuffer<BaseCommandBufferType>::beginRecording() {
        validateBeginRecording();
        BaseCommandBufferType::beginRecording();
    }

    template<typename BaseCommandBufferType>
    void ValidationTransferCommandBuffer<BaseCommandBufferType>::endRecording() {
        onEndRecording();
        BaseCommandBufferType::endRecording();
    }
}