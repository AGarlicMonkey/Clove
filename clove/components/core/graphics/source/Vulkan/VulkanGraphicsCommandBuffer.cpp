#include "Clove/Graphics/Vulkan/VulkanGraphicsCommandBuffer.hpp"

#include "Clove/Graphics/Vulkan/VulkanBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanCommandBuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanDescriptorSet.hpp"
#include "Clove/Graphics/Vulkan/VulkanFramebuffer.hpp"
#include "Clove/Graphics/Vulkan/VulkanGraphicsPipelineObject.hpp"
#include "Clove/Graphics/Vulkan/VulkanImage.hpp"
#include "Clove/Graphics/Vulkan/VulkanRenderPass.hpp"
#include "Clove/Graphics/Vulkan/VulkanShader.hpp"

#include <Clove/Cast.hpp>

namespace clove {
    namespace {
        VkIndexType getIndexType(IndexType garlicType) {
            switch(garlicType) {
                case IndexType::Uint16:
                    return VK_INDEX_TYPE_UINT16;
                default:
                    CLOVE_ASSERT(false, "{0}: Unkown index type", CLOVE_FUNCTION_NAME);
                    return VK_INDEX_TYPE_UINT16;
            }
        }

        template<typename... Ts>
        struct match : Ts... { using Ts::operator()...; };
        template<typename... Ts>
        match(Ts...) -> match<Ts...>;
    }

    VulkanGraphicsCommandBuffer::VulkanGraphicsCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyIndices queueFamilyIndices)
        : commandBuffer(commandBuffer)
        , queueFamilyIndices(queueFamilyIndices) {
    }

    VulkanGraphicsCommandBuffer::VulkanGraphicsCommandBuffer(VulkanGraphicsCommandBuffer &&other) noexcept = default;

    VulkanGraphicsCommandBuffer &VulkanGraphicsCommandBuffer::operator=(VulkanGraphicsCommandBuffer &&other) noexcept = default;

    VulkanGraphicsCommandBuffer::~VulkanGraphicsCommandBuffer() = default;

    void VulkanGraphicsCommandBuffer::beginRecording(CommandBufferUsage usageFlag) {
        VkCommandBufferBeginInfo beginInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags            = getCommandBufferUsageFlags(usageFlag),
            .pInheritanceInfo = nullptr,
        };

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Failed to begin recording command buffer");
        }
    }

    void VulkanGraphicsCommandBuffer::endRecording() {
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Failed to end recording command buffer");
        }
    }

    void VulkanGraphicsCommandBuffer::beginRenderPass(GhaRenderPass &renderPass, GhaFramebuffer &frameBuffer, RenderArea const &renderArea, std::span<ClearValue> clearValues) {
        std::vector<VkClearValue> vkClearValues(std::size(clearValues));
        for(uint32_t i{ 0 }; i < clearValues.size(); ++i) {
            std::visit(match{
                           [&](ColourValue const &colour) { vkClearValues[i].color = { colour.r, colour.g, colour.b, colour.a }; },
                           [&](DepthStencilValue const &depthStencil) { vkClearValues[i].depthStencil = { depthStencil.depth, depthStencil.stencil }; },
                       },
                       clearValues[i]);
        }

        VkRenderPassBeginInfo renderPassInfo{
            .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass  = polyCast<VulkanRenderPass>(&renderPass)->getRenderPass(),
            .framebuffer = polyCast<VulkanFramebuffer>(&frameBuffer)->getFrameBuffer(),
            .renderArea  = {
                .offset = { renderArea.origin.x, renderArea.origin.y },
                .extent = { renderArea.size.x, renderArea.size.y },
            },
            .clearValueCount = static_cast<uint32_t>(std::size(vkClearValues)),
            .pClearValues    = std::data(vkClearValues),
        };

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanGraphicsCommandBuffer::endRenderPass() {
        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanGraphicsCommandBuffer::setViewport(vec2i position, vec2ui size) {
        VkViewport const viewport{
            .x        = static_cast<float>(position.x),
            .y        = static_cast<float>(position.y),
            .width    = static_cast<float>(size.x),
            .height   = static_cast<float>(size.y),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    void VulkanGraphicsCommandBuffer::setScissor(vec2i position, vec2ui size) {
        VkRect2D const scissor{
            .offset = { position.x, position.y },
            .extent = { size.x, size.y },
        };

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanGraphicsCommandBuffer::bindPipelineObject(GhaGraphicsPipelineObject &pipelineObject) {
        auto const *pipeline{ polyCast<VulkanGraphicsPipelineObject>(&pipelineObject) };

        currentLayout = pipeline->getLayout();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());
    }

    void VulkanGraphicsCommandBuffer::bindVertexBuffer(GhaBuffer &vertexBuffer, size_t const offset) {
        VkBuffer const buffer{ polyCast<VulkanBuffer>(&vertexBuffer)->getBuffer() };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &offset);
    }

    void VulkanGraphicsCommandBuffer::bindIndexBuffer(GhaBuffer &indexBuffer, size_t const offset, IndexType indexType) {
        vkCmdBindIndexBuffer(commandBuffer, polyCast<VulkanBuffer>(&indexBuffer)->getBuffer(), offset, getIndexType(indexType));
    }

    void VulkanGraphicsCommandBuffer::bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) {
        VkDescriptorSet const set{ polyCast<VulkanDescriptorSet>(&descriptorSet)->getDescriptorSet() };

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentLayout, setNum, 1, &set, 0, nullptr);
    }

    void VulkanGraphicsCommandBuffer::pushConstant(GhaShader::Stage const stage, size_t const offset, size_t const size, void const *data) {
        vkCmdPushConstants(commandBuffer, currentLayout, VulkanShader::convertStage(stage), offset, size, data);
    }

    void VulkanGraphicsCommandBuffer::drawIndexed(size_t const indexCount) {
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanGraphicsCommandBuffer::bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
        createBufferMemoryBarrier(commandBuffer, queueFamilyIndices, buffer, barrierInfo, sourceStage, destinationStage);
    }

    void VulkanGraphicsCommandBuffer::imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
        createImageMemoryBarrier(commandBuffer, queueFamilyIndices, image, barrierInfo, sourceStage, destinationStage);
    }

    VkCommandBuffer VulkanGraphicsCommandBuffer::getCommandBuffer() const {
        return commandBuffer;
    }
}