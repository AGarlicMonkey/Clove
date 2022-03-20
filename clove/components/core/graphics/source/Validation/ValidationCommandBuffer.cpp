#include "Clove/Graphics/Validation/ValidationCommandBuffer.hpp"

#include <Clove/Log/Log.hpp>

namespace clove {
    void ValidationCommandBuffer::markAsSubmitted() {
        hasBeenSubmitted = true;
    }

    bool ValidationCommandBuffer::hasBufferBeenSubmitted() const {
        return hasBeenSubmitted;
    }

    void ValidationCommandBuffer::validateBeginRecording() {
        CLOVE_ASSERT_MSG(endRecordingCalled, "beginRecording called before endRecording. Command buffer recording must be finished be starting again.");
        endRecordingCalled = false;

        CLOVE_ASSERT_MSG(!hasBeenRecorded, "Command buffer re-recorded to. Command buffers cannot be recorded to more than once.");
        CLOVE_ASSERT_MSG(!hasBeenSubmitted, "Command buffer has already been submitted. Cannot being recording on a command buffer once it has been submitted.");
    }

    void ValidationCommandBuffer::onEndRecording() {
        hasBeenRecorded    = false;
        endRecordingCalled = true;
    }
}

