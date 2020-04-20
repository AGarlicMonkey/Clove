#pragma once

#include "Bulb/ECS/Component.hpp"

#include "Clove/Graphics/GraphicsTypes.hpp"
#include "Bulb/Rendering/ShaderBufferTypes.hpp"

namespace clv::gfx {
	class RenderTarget;
}

namespace clv::plt {
	class Window;
}

namespace blb::ecs {
	enum class ProjectionMode {
		orthographic,
		perspective
	};
}

namespace blb::ecs {
	class CameraComponent : public Component<CameraComponent> {
		friend class RenderSystem;

		//VARIABLES
	private:
		ProjectionMode currentProjectionMode;
		clv::mth::mat4f currentProjection = clv::mth::mat4f{ 1.0f };
		clv::mth::mat4f currentView = clv::mth::mat4f{ 1.0f };

		clv::mth::vec3f cameraUp = { 0.0f, 1.0f, 0.0f };
		clv::mth::vec3f cameraFront = { 0.0f, 0.0f, 1.0f };
		clv::mth::vec3f cameraRight = { 1.0f, 0.0f, 0.0f };

		blb::rnd::CameraRenderData cameraRenderData;

		std::shared_ptr<clv::gfx::RenderTarget> renderTarget;
		clv::gfx::Viewport viewport;

		//FUNCTIONS
	public:
		CameraComponent(std::shared_ptr<clv::gfx::RenderTarget> renderTarget, const clv::gfx::Viewport& viewport, const ProjectionMode projection);
		CameraComponent(clv::plt::Window& window, const ProjectionMode projection);

		CameraComponent(const CameraComponent& other);
		CameraComponent(CameraComponent&& other) noexcept;

		CameraComponent& operator=(const CameraComponent& other);
		CameraComponent& operator=(CameraComponent&& other) noexcept;

		~CameraComponent();

		const clv::mth::mat4f& getProjection() const;
		const clv::mth::mat4f& getView() const;

		const clv::mth::vec3f& getFront() const;
		const clv::mth::vec3f& getUp() const;
		const clv::mth::vec3f& getRight() const;

		const clv::gfx::Viewport& getViewport() const;

		void setProjectionMode(const ProjectionMode mode);
		ProjectionMode getProjectionMode() const;

		void updateViewportSize(const clv::mth::vec2ui& viewportSize);
	};
}