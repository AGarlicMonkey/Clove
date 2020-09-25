#pragma once

#include "Bulb/ECS/Component.hpp"

#include "Bulb/Rendering/ShaderBufferTypes.hpp"

namespace blb::ecs {
	class DirectionalLightComponent : public Component<DirectionalLightComponent> {
		friend class RenderSystem;

		//VARIABLES
	private:
		rnd::DirectionalLight lightData;

		static constexpr float size = 50.0f;
		static constexpr float nearDist = 0.5f;
		static constexpr float farDist = 1000.0f;
		clv::mth::mat4f shadowProj = clv::mth::createOrthographicMatrix(-size, size, -size, size, nearDist, farDist);

		//FUNCTIONS
	public:
		DirectionalLightComponent();

		DirectionalLightComponent(const DirectionalLightComponent& other);
		DirectionalLightComponent(DirectionalLightComponent&& other) noexcept;

		DirectionalLightComponent& operator=(const DirectionalLightComponent& other);
		DirectionalLightComponent& operator=(DirectionalLightComponent&&) noexcept;

		~DirectionalLightComponent();

		/**
		 * @brief Sets the direction the light faces.
		 */
		void setDirection(const clv::mth::vec3f& direction);
		const clv::mth::vec3f& getDirection() const;
	};
}