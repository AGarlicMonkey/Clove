#include "Clove/Graphics/Validation/ValidationCommandBuffer.hpp"

#include <Clove/Log/Log.hpp>

namespace clove {
    void ValidationCommandBuffer::markAsUsed() {
        hasBeenUsed = true;
    }

    bool ValidationCommandBuffer::bufferHasBeenUsed() const {
        return hasBeenUsed;
    }

    void ValidationCommandBuffer::validateBeginRecording() {
        CLOVE_ASSERT_MSG(endRecordingCalled, "beginRecording called before endRecording. Command buffer recording must be finished be starting again.");
        endRecordingCalled = false;

        CLOVE_ASSERT_MSG(!hasBeenUsed, "Command buffer re-recorded to. Command buffers cannot be recorded to more than once unless the owning queue has been created with QueueFlags::ReuseBuffers set.");
    }

    void ValidationCommandBuffer::resetUsedFlag() {
        hasBeenUsed = false;
    }

    void ValidationCommandBuffer::onEndRecording() {
        endRecordingCalled = true;
    }
}

