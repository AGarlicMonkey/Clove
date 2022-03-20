#pragma once

#include "Clove/Graphics/GhaComputeCommandBuffer.hpp"
#include "Clove/Graphics/GhaGraphicsCommandBuffer.hpp"
#include "Clove/Graphics/GhaTransferCommandBuffer.hpp"

namespace clove {
    class ValidationCommandBuffer {
        //VARIABLES
    private:
        bool hasBeenRecorded{ false };
        bool hasBeenSubmitted{ false };
        bool endRecordingCalled{ true };

        //FUNCTIONS
    public:
        void markAsSubmitted();
        bool hasBufferBeenSubmitted() const;

    protected:
        void validateBeginRecording();

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
