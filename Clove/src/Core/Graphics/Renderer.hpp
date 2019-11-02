#pragma once

#include "Core/Graphics/GraphicsTypes.hpp"

#include "Core/Graphics/Bindables/ShaderBufferObject.hpp"
#include "Core/Graphics/GraphicsConstants.hpp"
#include "Core/Graphics/MaterialInstance.hpp"

namespace clv::gfx{
	class RenderTarget;
	class Mesh;
	struct CameraRenderData;
	struct PointLightData;

	class Renderer{
		//FUNCTIONS
	public:
		static void initialise();
		static void shutDown();

		static void beginScene();
		static void endScene();

		static void setRenderTarget(const std::shared_ptr<RenderTarget>& inRenderTarget);
		static void clearRenderTargets();
		static void removeRenderTarget();

		static void submitMesh(const std::shared_ptr<Mesh>& mesh);
		static void setCamera(const CameraRenderData& data);
		static void submitPointLight(const PointLightData& data);
	};
}