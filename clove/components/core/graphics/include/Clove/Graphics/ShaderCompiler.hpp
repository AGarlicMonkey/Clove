#pragma once

#include "Clove/Graphics/GhaShader.hpp"

#include <Clove/Expected.hpp>
#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <Clove/Maths/Vector.hpp>

namespace clove::ShaderCompiler {
    /**
     * @brief Compiles a GLSL shader file into SPIR-V shader byte code. 
     * @details All includes will be searched for as relative paths.
     * @param file Absolute path to the file.
     * @param shaderStage The stage that's being compiled.
     * @param outputType Which type of shader code to compile to.
     * @return SPIR-V shader byte code.
     */
    Expected<std::vector<uint32_t>, std::runtime_error> compileFromFile(std::filesystem::path const &file, GhaShader::Stage shaderStage);

    /**
     * @brief Compiles a GLSL shader source string into SPIR-V shader byte code.
     * @param source std::string_view of the GhaShader source.
     * @param includeSources std::unordered map of shader source strings to use as includes. Key is the name of the include and value is the string of the include.
     * @param shaderName The name of the shader being compiled. Used for debug purposes, can be empty.
     * @param shaderStage The stage that's being compiled.
     * @param outputType Which type of shader code to compile to.
     * @return SPIR-V shader byte code.
     */
    Expected<std::vector<uint32_t>, std::runtime_error> compileFromSource(std::string_view source, std::unordered_map<std::string, std::string> includeSources, std::string_view shaderName, GhaShader::Stage shaderStage);

    /**
     * @brief Transpiles SPIR-V shader code into HLSL shader code.
     * @return HLSL shader code as a string.
     */
    std::string spirvToHLSL(std::span<uint32_t> spirvSource);
    /**
     * @brief Transpiles SPIR-V shader code into MSL shader code.
     * @return MSL shader code as a string.
     */
    std::string spirvToMSL(std::span<uint32_t> spirvSource);
    
    /**
     * @brief Returns the workgroup size of a given shader.
     * @return
     */
    vec3ui getWorkgroupSize(std::span<uint32_t> spirvSource);
}
