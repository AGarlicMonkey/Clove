#include "Clove/Components/RigidBodyComponent.hpp"

namespace clove{
    void RigidBodyComponent::applyForce(vec3f force, vec3f relativeOffset) {
        appliedForce = ForceApplication{ .amount = force, .offset = relativeOffset };
    }

    void RigidBodyComponent::applyImpulse(vec3f impulse, vec3f relativeOffset) {
        appliedImpulse = ForceApplication{ .amount = impulse, .offset = relativeOffset };
    }
}