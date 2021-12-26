#pragma once

#include "Clove/Rendering/RenderGraph/RgBindings.hpp"
#include "Clove/Rendering/RenderGraph/RgId.hpp"
#include "Clove/Rendering/RenderGraph/RgPass.hpp"
#include "Clove/Rendering/RenderGraph/RgShader.hpp"

#include <Clove/Graphics/GhaGraphicsPipelineObject.hpp>

#include <vector>

namespace clove {
    class RgGlobalCache;
}

namespace clove {
    /**
     * @brief Represents an entire render pass in the graph.
     */
    class RgRenderPass : public RgPass {
        //TYPES
    public:
        struct Descriptor {
            RgShader vertexShader{};
            RgShader pixelShader{};

            VertexInputBindingDescriptor vertexInput{};
            std::vector<VertexAttributeDescriptor> vertexAttributes{}; /**< The index of each element maps to the layout(location = x) in the vertex shader. */

            vec2i viewportPosition{ 0 };
            vec2ui viewportSize{ 0 };
            //TODO: raster state
            bool depthTest{ true };
            bool depthWrite{ true };
            bool enableBlending{ true };

            std::vector<RgRenderTargetBinding> renderTargets{};
            RgDepthStencilBinding depthStencil{};
        };

        /**
         * @brief Represents a unit of work, single draw indexed call.
         */
        struct Submission {
            RgResourceId vertexBuffer{};
            RgResourceId indexBuffer{};

            std::vector<RgBufferBinding> readUniformBuffers{};
            std::vector<RgBufferBinding> readStorageBuffers{};

            std::vector<RgImageBinding> images{};
            std::vector<RgSamplerBinding> samplers{};

            size_t indexCount{ 0 };
        };

        //VARIABLES
    private:
        Descriptor descriptor{};
        std::vector<Submission> submissions{};

        //FUNCTIONS
    public:
        RgRenderPass() = delete;
        RgRenderPass(Descriptor descriptor);

        RgRenderPass(RgRenderPass const &other) = delete;
        RgRenderPass(RgRenderPass &&other) noexcept;

        RgRenderPass &operator=(RgRenderPass const &other) = delete;
        RgRenderPass &operator=(RgRenderPass &&other) noexcept;

        ~RgRenderPass();

        std::unordered_set<RgResourceId> getInputResources() const override;
        std::unordered_set<RgResourceId> getOutputResources() const override;

        inline void addSubmission(Submission submission);

        inline Descriptor const &getDescriptor() const;
        inline std::vector<Submission> const &getSubmissions() const;
    };
}

#include "RgRenderPass.inl"