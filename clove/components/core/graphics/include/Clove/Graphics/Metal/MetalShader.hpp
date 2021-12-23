#pragma once

#include "Clove/Graphics/GhaShader.hpp"

#include <Clove/Maths/Vector.hpp>
#include <MetalKit/MetalKit.h>

namespace clove {
	class MetalShader : public GhaShader {
		//VARIABLES
	private:
		id<MTLFunction> function;
        
        vec3ui workgroupSize{}; //TEMP: Storing workgroup sizes inside all metal shaders. These are defined outside of the shader unlike D3D and Vulkan (see MetalComputeCommandEncoder.mm)
		
		//FUNCTIONS
	public:
		MetalShader() = delete;
		MetalShader(id<MTLFunction> function, vec3ui workgroupSize);
		
		MetalShader(MetalShader const &other) = delete;
		MetalShader(MetalShader &&other) noexcept;
		
		MetalShader& operator=(MetalShader const &other) = delete;
		MetalShader& operator=(MetalShader &&other) noexcept;
		
		~MetalShader();
		
		inline id<MTLFunction> getFunction() const;
        inline vec3ui const &getWorkgroupSize() const;
	};
}

#include "MetalShader.inl"
