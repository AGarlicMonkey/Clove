#pragma once

#include "Clove/Graphics/GhaComputeCommandBuffer.hpp"
#include "Clove/Graphics/GhaGraphicsCommandBuffer.hpp"
#include "Clove/Graphics/GhaTransferCommandBuffer.hpp"

namespace clove {
    class ValidationCommandBuffer {
        //VARIABLES
    private:
        bool hasBeenUsed{ false }; /**< Will be true if this buffer has been used before being rerecorded. */
        bool endRecordingCalled{ true };

        //FUNCTIONS
    public:
        void markAsUsed();
        bool bufferHasBeenUsed() const;

    protected:
        void validateBeginRecording();
        void resetUsedFlag();

        void onEndRecording();
    };

    template<typename BaseCommandBufferType>
    class ValidationGraphicsCommandBuffer : public BaseCommandBufferType, public ValidationCommandBuffer {
        //FUNCTIONS
    public:
        using BaseCommandBufferType::BaseCommandBufferType;

        void beginRecording() override;
        void endRecording() override;
    };

    template<typename BaseCommandBufferType>
    class ValidationComputeCommandBuffer : public BaseCommandBufferType, public ValidationCommandBuffer {
        //FUNCTIONS
    public:
        using BaseCommandBufferType::BaseCommandBufferType;

        void beginRecording() override;
        void endRecording() override;
    };

    template<typename BaseCommandBufferType>
    class ValidationTransferCommandBuffer : public BaseCommandBufferType, public ValidationCommandBuffer {
        //FUNCTIONS
    public:
        using BaseCommandBufferType::BaseCommandBufferType;

        void beginRecording() override;
        void endRecording() override;
    };
}

#include "ValidationCommandBuffer.inl"
