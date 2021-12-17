#pragma once

#include "Clove/Rendering/RenderPasses/GeometryPass.hpp"
#include "Clove/Rendering/ShaderBufferTypes.hpp"

#include <memory>
#include <set>

namespace clove {
    class Mesh;
    class Material;
    class GhaImage;
}

namespace clove {
    class Renderer {
        //TYPES
    public:
        //TODO: Currently transform and matrixPalet are copied per mesh for each model. This should be avoided
        struct MeshInfo {
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<Material> material;
            mat4f transform;
            std::array<mat4f, MAX_JOINTS> matrixPalet;

            std::set<GeometryPass::Id> geometryPassIds;
        };

        //FUNCTIONS
    public:
        virtual ~Renderer() = default;

        virtual void begin() = 0;

        virtual void submitMesh(MeshInfo meshInfo) = 0;

        /**
         * @brief Submit the active camera the renderer will use.
         */
        virtual void submitCamera(mat4f const view, mat4f const projection, vec3f const position) = 0;

        virtual void submitLight(DirectionalLight const &light) = 0;
        virtual void submitLight(PointLight const &light)       = 0;

        virtual void submitWidget(std::shared_ptr<GhaImage> widget, mat4f const modelProjection) = 0;
        virtual void submitText(std::shared_ptr<GhaImage> text, mat4f const modelProjection)     = 0;

        virtual void end() = 0;

        virtual vec2ui getRenderTargetSize() const = 0;
    };
}