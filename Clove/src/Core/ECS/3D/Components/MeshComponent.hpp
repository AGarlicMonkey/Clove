#pragma once

#include "Core/ECS/Component.hpp"

#include "Core/Utils/HashString.hpp"

namespace clv::gfx{
	class Mesh;
}

namespace clv::ecs::_3D{
	class MeshComponent : public Component{
		friend class RenderSystem;

		//VARIABLES
	public:
		static constexpr ComponentID ID = HASH_CLASS(MeshComponent);

	private:
		std::shared_ptr<gfx::Mesh> mesh; 

		//FUNCTIONS
	public:
		MeshComponent();
		MeshComponent(const MeshComponent& other) = delete;
		MeshComponent(MeshComponent&& other) noexcept;
		MeshComponent& operator=(const MeshComponent& other) = delete;
		MeshComponent& operator=(MeshComponent&& other) noexcept;
		virtual ~MeshComponent();

		void setMesh(const std::shared_ptr<gfx::Mesh>& mesh);
	};
}