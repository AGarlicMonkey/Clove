#include "Clove/Graphics/Vulkan/VulkanPipelineObject.hpp"

#include <Clove/Log/Log.hpp>

namespace clove {
    VkPipelineStageFlags convertStage(PipelineStage stage) {
        switch(stage) {
            case PipelineStage::Top:
                return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            case PipelineStage::Transfer:
                return VK_PIPELINE_STAGE_TRANSFER_BIT;
            case PipelineStage::ComputeShader:
                return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            case PipelineStage::VertexInput:
                return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            case PipelineStage::VertexShader:
                return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            case PipelineStage::PixelShader:
                return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            case PipelineStage::EarlyPixelTest:
                return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            case PipelineStage::LatePixelTest:
                return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            case PipelineStage::ColourAttachmentOutput:
                return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            default:
                CLOVE_ASSERT(false, "{0}: Unkown pipeline stage", CLOVE_FUNCTION_NAME);
                return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
    }
}