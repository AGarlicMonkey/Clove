#pragma once

#include <Clove/Definitions.hpp>
#include <cinttypes>

namespace clove {
    /**
     * @brief A GhaShader represents a small piece of code that runs on the GPU
     */
    class GhaShader {
        //TYPES
    public:
        using StageType = uint16_t;
        enum class Stage : StageType {
            Vertex  = 1 << 0,
            Pixel   = 1 << 1,
            Compute = 1 << 2,
        };

        //FUNCTIONS
    public:
        virtual ~GhaShader() = default;
    };

    CLOVE_ENUM_BIT_FLAG_OPERATORS(GhaShader::Stage, GhaShader::StageType)
}