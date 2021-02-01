#include "Clove/Rendering/RenderPasses/DirectionalLightPass.hpp"

#include "Clove/Rendering/Renderables/Mesh.hpp"
#include "Clove/Rendering/RenderingHelpers.hpp"
#include "Clove/Rendering/Vertex.hpp"

#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaGraphicsCommandBuffer.hpp>
#include <Clove/Graphics/GhaPipelineObject.hpp>
#include <Clove/Graphics/GhaRenderPass.hpp>

extern "C" const char constants[];
extern "C" const size_t constantsLength;

//extern "C" const char staticmeshshadowmap_v[];
//extern "C" const size_t staticmeshshadowmap_vLength;
extern "C" const char animatedmeshshadowmap_v[];
extern "C" const size_t animatedmeshshadowmap_vLength;

extern "C" const char meshshadowmap_p[];
extern "C" const size_t meshshadowmap_pLength;

namespace garlic::clove {
    DirectionalLightPass::DirectionalLightPass(GhaFactory &ghaFactory, std::shared_ptr<GhaRenderPass> ghaRenderPass) {
        //Build include map
        std::unordered_map<std::string, std::string> shaderIncludes;
        shaderIncludes["Constants.glsl"] = { constants, constantsLength };

        //Pipeline
        std::vector<VertexAttributeDescriptor> const vertexAttributes{
            VertexAttributeDescriptor{
                .location = 0,
                .format   = VertexAttributeFormat::R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex, position),
            },
            VertexAttributeDescriptor{
                .location = 1,
                .format   = VertexAttributeFormat::R32G32B32A32_SINT,
                .offset   = offsetof(Vertex, jointIds),
            },
            VertexAttributeDescriptor{
                .location = 2,
                .format   = VertexAttributeFormat::R32G32B32A32_SFLOAT,
                .offset   = offsetof(Vertex, weights),
            }
        };

        PushConstantDescriptor const pushConstant{
            .stage = GhaShader::Stage::Vertex,
            .size  = sizeof(mat4f),
        };

        AreaDescriptor const viewScissorArea{
            .state    = ElementState::Static,
            .position = { 0.0f, 0.0f },
            .size     = { shadowMapSize, shadowMapSize }
        };

        pipeline = *ghaFactory.createPipelineObject(GhaPipelineObject::Descriptor{
            .vertexShader         = *ghaFactory.createShaderFromSource({ animatedmeshshadowmap_v, animatedmeshshadowmap_vLength }, shaderIncludes, "Shadow Map - Animated Mesh (vertex)", GhaShader::Stage::Vertex),
            .fragmentShader       = *ghaFactory.createShaderFromSource({ meshshadowmap_p, meshshadowmap_pLength }, shaderIncludes, "Shadow Map (pixel)", GhaShader::Stage::Pixel),
            .vertexInput          = Vertex::getInputBindingDescriptor(),
            .vertexAttributes     = std::move(vertexAttributes),
            .viewportDescriptor   = viewScissorArea,
            .scissorDescriptor    = viewScissorArea,
            .enableBlending       = false,
            .renderPass           = std::move(ghaRenderPass),
            .descriptorSetLayouts = {
                createMeshDescriptorSetLayout(ghaFactory),
            },
            .pushConstants = { 
                std::move(pushConstant),
            },
        });
    }

    DirectionalLightPass::DirectionalLightPass(DirectionalLightPass &&other) noexcept = default;

    DirectionalLightPass &DirectionalLightPass::operator=(DirectionalLightPass &&other) noexcept = default;

    DirectionalLightPass::~DirectionalLightPass() = default;

    void DirectionalLightPass::addJob(Job job) {
        jobs.emplace_back(std::move(job));
    }

    void DirectionalLightPass::flushJobs() {
        jobs.clear();
    }

    void DirectionalLightPass::execute(GhaGraphicsCommandBuffer &commandBuffer, FrameData const &frameData) {
        commandBuffer.bindPipelineObject(*pipeline);

        for(auto &job : jobs) {
            mat4f const *pushConstantData{ frameData.currentDirLightTransform };
            size_t const pushConstantSize{ sizeof(mat4f) };

            commandBuffer.pushConstant(GhaShader::Stage::Vertex, 0, pushConstantSize, pushConstantData);
            commandBuffer.bindDescriptorSet(*frameData.meshDescriptorSets[job.meshDescriptorIndex], 0);

            commandBuffer.bindVertexBuffer(*job.mesh->getGhaBuffer(), job.mesh->getVertexOffset());
            commandBuffer.bindIndexBuffer(*job.mesh->getGhaBuffer(), job.mesh->getIndexOffset(), IndexType::Uint16);

            commandBuffer.drawIndexed(job.mesh->getIndexCount());
        }
    }
}