#include "Clove/Graphics/Graphics.hpp"

#if CLV_PLATFORM_WINDOWS
	#include "Clove/Graphics/OpenGL/GL.hpp"
	#include "Clove/Graphics/Direct3D/D3D.hpp"
	#include "Clove/Graphics/Vulkan/VK.hpp"
#elif CLV_PLATFORM_LINUX
	#include "Clove/Graphics/OpenGL/GL.hpp"
	#include "Clove/Graphics/Vulkan/VK.hpp"
#elif CLV_PLATFORM_MACOS
	#include "Clove/Graphics/Metal/MTL.hpp"
#endif

namespace clv::gfx{
	std::shared_ptr<GraphicsFactory> initialise(API api) {
		switch(api){
			#if CLV_PLATFORM_WINDOWS
			case API::OpenGL4:
				CLV_LOG_TRACE("Creating OpenGL renderer backend");
				return ogl::initialiseOGL();

			case API::Direct3D11:
				CLV_LOG_TRACE("Creating Direct3D renderer backend");
				return d3d::initialiseD3D();

			case API::Vulkan:
				CLV_LOG_TRACE("Creating Vulkan renderer backend");
				return d3d::initialiseD3D();

			#elif CLV_PLATFORM_LINUX
			case API::OpenGL4:
				CLV_LOG_TRACE("Creating OpenGL renderer backend");
				return ogl::initialiseOGL();

			case API::Vulkan:
				CLV_LOG_TRACE("Creating Vulkan renderer backend");
				return d3d::initialiseD3D();
				
			#elif CLV_PLATFORM_MACOS
			case API::Metal1:
				CLV_LOG_TRACE("Creating Metal renderer backend");
				return mtl::initialiseMTL();
			#endif

			default:
				CLV_LOG_ERROR("Default statement hit. Could not initialise RenderAPI: {0}", CLV_FUNCTION_NAME);
				return std::shared_ptr<GraphicsFactory>();
		}
	}
}
