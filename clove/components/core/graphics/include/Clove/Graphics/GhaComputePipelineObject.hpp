#pragma once

#include "Clove/Graphics/PipelineObject.hpp"

#include <memory>
#include <vector>

namespace garlic::clove {
    class GhaDescriptorSetLayout;
    class GhaShader;
}

namespace garlic::clove {
    class GhaComputePipelineObject {
        //TYPES
    public:
        struct Descriptor {
            std::shared_ptr<GhaShader> shader;

            std::vector<std::shared_ptr<GhaDescriptorSetLayout>> descriptorSetLayouts;
            std::vector<PushConstantDescriptor> pushConstants;
        };

        //FUNCTIONS
    public:
        virtual ~GhaComputePipelineObject() = default;
    };
}