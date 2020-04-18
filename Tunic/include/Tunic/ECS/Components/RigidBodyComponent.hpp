#pragma once

#include "Tunic/ECS/Component.hpp"

namespace tnc::phy {
	class RigidBody;
}

namespace tnc::ecs {
	class RigidBodyComponent : public Component<RigidBodyComponent> {
		friend class PhysicsSystem;

		//VARIABLES
	public:
		clv::utl::MultiCastDelegate<void(RigidBodyComponent*)> onBodyCollision;

	private:
		std::unique_ptr<phy::RigidBody> rigidBody;

		//FUNCTIONS
	public:
		RigidBodyComponent();
		RigidBodyComponent(float mass, bool isKinematic, bool respondToCollision, const clv::mth::vec3f& cubeSize);
		RigidBodyComponent(std::unique_ptr<phy::RigidBody> rigidBody);

		RigidBodyComponent(const RigidBodyComponent& other);
		RigidBodyComponent(RigidBodyComponent&& other) noexcept;

		RigidBodyComponent& operator=(const RigidBodyComponent& other);
		RigidBodyComponent& operator=(RigidBodyComponent&& other) noexcept;

		virtual ~RigidBodyComponent();

	private:
		void initialiseRigidBody(phy::RigidBody* body);
	};
}