#pragma once

#include "Clove/Rendering/RenderGraph/RgBindings.hpp"
#include "Clove/Rendering/RenderGraph/RgId.hpp"
#include "Clove/Rendering/RenderGraph/RgPass.hpp"
#include "Clove/Rendering/RenderGraph/RgShader.hpp"

#include <Clove/Maths/Vector.hpp>
#include <vector>

namespace clove {
    class RgComputePass : public RgPass {
        //TYPES
    public:
        struct Descriptor {
            RgShader shader{};
        };

        /**
         * @brief Represents a single unit of work. A single dispatch
         */
        struct Submission {
            std::vector<RgBufferBinding> readUniformBuffers{};
            std::vector<RgBufferBinding> readStorageBuffers{};

            std::vector<RgBufferBinding> writeBuffers{};

            std::vector<RgImageBinding> readImages{};/**< Read images will require samplers */
            std::vector<RgImageBinding> writeImages{}; 

            std::vector<RgSamplerBinding> samplers{};

            vec3ui disptachSize{};
        };

        //VARIABLES
    private:
        Descriptor descriptor{};
        std::vector<Submission> submissions{};

        //FUNCTIONS
    public:
        RgComputePass() = delete;
        RgComputePass(Descriptor descriptor);

        RgComputePass(RgComputePass const &other) = delete;
        RgComputePass(RgComputePass &&other) noexcept;

        RgComputePass &operator=(RgComputePass const &other) = delete;
        RgComputePass &operator=(RgComputePass &&other) noexcept;

        ~RgComputePass();

        std::unordered_set<RgResourceId> getInputResources() const override;
        std::unordered_set<RgResourceId> getOutputResources() const override;

        inline void addSubmission(Submission submission);

        inline Descriptor const &getDescriptor() const;
        inline std::vector<Submission> const &getSubmissions() const;
    };
}

#include "RgComputePass.inl"