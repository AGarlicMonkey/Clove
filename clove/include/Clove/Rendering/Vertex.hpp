#pragma once

#include <Clove/Graphics/GhaGraphicsPipelineObject.hpp>
#include <Clove/Maths/Vector.hpp>

namespace clove {
    inline size_t constexpr vertexElementAlignment{ 16 };
    struct Vertex {
        alignas(vertexElementAlignment) vec3f position{ 0.0f };
        alignas(vertexElementAlignment) vec3f normal{ 0.0f };
        alignas(vertexElementAlignment) vec2f texCoord{ 0.0f };
        alignas(vertexElementAlignment) vec3f colour{ 0.0f };
        alignas(vertexElementAlignment) vec4i jointIds{ 0 };
        alignas(vertexElementAlignment) vec4f weights{ 1.0f / 4.0f };//We need to divide by 4 here else we'll accumulate 4 identity matricies for static meshes

        static VertexInputBindingDescriptor getInputBindingDescriptor() {
            return VertexInputBindingDescriptor{
                .stride = sizeof(Vertex),
            };
        }
    };
}